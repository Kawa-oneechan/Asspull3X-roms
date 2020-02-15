#include "../ass.h"
#include "game.h"
extern unsigned int rndseed;
extern void srand(unsigned int seed);
extern unsigned int rand();

#define KEY_UP 0xC8
#define KEY_LEFT 0xCB
#define KEY_RIGHT 0xCD
#define KEY_DOWN 0xD0

/*
 * reset_game:  sets game fields to their initial state.
 */
static void reset_game(game *game);

/*
 * spawn_next_on_top:  changes the shape for game->falling with game->next_shape
 *		     and tries to spawn game->falling on top of the grid. On
 *		     success, puts another random shape in game->next_shape
 *		     and returns true, on failure, returns false.
 */
static int spawn_next_on_top(game *game);

/*
 * update_next_shape:  puts the next random shape in game->next_shape.
 *		     If all shapes in game->next_shapes have been used,
 *		     shuffles the array game->next_shapes before change
 *		     game->next_shape.
 *
 *		     http://tetris.wikia.com/wiki/Random_Generator
 */
static void update_next_shape(game *game);

/*
 * update_infos:  redraw the stuff outside of the grid.
 */
static void update_infos(game *game);

/*
 * get_action:  waits for a key to be pressed by the user for at most delay
 *	      milliseconds and returns the next action to be performed
 *	      according to the key pressed.
 *
 *	      If the key is not valid or no key was pressed within delay
 *	      milliseconds, MOVE_DOWN will be returned.
 *
 *	      A negative delay will cause to wait indefinitely until a key
 *	      will be pressed.
 */
static action get_action(int delay);

/*
 * push_in_history:  puts action in game->history_moves overwriting the "older"
 *		   move.
 */
static void push_in_history(action action, game *game);

/*
 * pause_game:  puts the game on pause waiting for the pause or end game key.
 */
static void pause_game(game *game);

/*
 * remove_full_lines:  scans the grid looking for rows without empty blocks and
 *		     removes them. Returns the number of full lines removed.
 */
static int remove_full_lines(game *game);

/*
 * increase_level:  increases game->level by 1 and reduces game->delay if
 *		  (game->lines / 10) + 1 is greater than game->level.
 */
static void increase_level(game *game);

/*
 * increase_score:  increases game->score according to lines (the number of
 *		  lines removed at the same time) and the current level.
 *
 *		  Applies the formula m * (n + 1) where n is level and m takes
 *		  the value of 40, 100, 300, 1200 depending on lines
 *		  (1 -> 40, 2 -> 100 ...).
 */
static void increase_score(game *game, int lines);


game *init_game(void)
{
	srand(*(signed long long*)(MEM_IO + 0x0060)); //time(NULL));

	game *new_game = malloc(sizeof(*new_game));

	new_game->grid = create_grid(GRID_ROWS, GRID_COLS);
	new_game->falling = create_tetramino();

	memcpy(new_game->next_shapes, shapes_letters, TOT_SHAPES);

	reset_game(new_game);

	return new_game;
}

static void reset_game(game *game)
{
	flush_grid(game->grid);
	game->falling->on_grid = 0;

	game->next_index = 0;
	update_next_shape(game);

	game->level = 1;
	game->score = 0;
	game->lines = 0;
	game->delay = STARTING_DELAY;

	set_array(MOVE_DOWN, game->history_moves, HISTORY_SIZE);
	game->history_index = 0;

	game->is_paused = 0;
	game->is_over = 0;
}

void run_game(game *game)
{
	action action;
	int lines;

	while (!game->is_over)
	{
		if (!game->falling->on_grid)
		{
			if (!spawn_next_on_top(game))
			{
				game->is_over = 1;
				continue;
			}
		}

		update_infos(game);
		refresh_grid(game->grid);

		if (!is_in_array(MOVE_DOWN, game->history_moves, HISTORY_SIZE))
		{
			perform_action(game->grid, game->falling, MOVE_DOWN);
			push_in_history(MOVE_DOWN, game);
		}
		else
		{
			action = get_action(game->delay);
			if (action == PAUSE)
			{
				pause_game(game);
				continue;
			}
			else
			{
				perform_action(game->grid, game->falling, action);
				push_in_history(action, game);
			}
		}

		if (game->falling->is_locked)
		{
			lines = remove_full_lines(game);
			if (lines)
			{
				game->lines += lines;
				increase_level(game);
				increase_score(game, lines);
			}
		}
	}
}

static int spawn_next_on_top(game *game)
{
	set_array(MOVE_DOWN, game->history_moves, HISTORY_SIZE);

	change_shape(game->falling, game->next_shape);

	game->falling->row = 0;
	game->falling->col = game->grid->cols / 2 - game->falling->size / 2;

	if (!check_position(game->grid, game->falling, game->falling->row, game->falling->col))
		return 0;

	put_on_grid(game->grid, game->falling, game->falling->row, game->falling->col);
	update_next_shape(game);
	return 1;
}

static void update_next_shape(game *game)
{
	if (game->next_index == 0)
		shuffle_array(game->next_shapes, TOT_SHAPES);

	game->next_shape = game->next_shapes[game->next_index];
	game->next_index = (game->next_index + 1) % TOT_SHAPES;
}

#include "tetramino.h"
static void update_infos(game *game)
{
	int next_shape;
	switch (game->next_shape)
	{
		case 'I': next_shape = 0; break;
		case 'J': next_shape = 1; break;
		case 'L': next_shape = 2; break;
		case 'O': next_shape = 3; break;
		case 'S': next_shape = 4; break;
		case 'T': next_shape = 5; break;
		case 'Z': next_shape = 6; break;
	}
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			MAP2[((i + 2) * 64) + j + 18] = (shapes[next_shape][0][i][j]) ? (next_shape + 1) : 0;

	char buffer[64];
	sprintf(buffer, "%7d", game->score);
	uint16_t *here = &MAP1[(15 * 64) + 28];
	char *c = buffer;
	while (*c) *here++ = 0x2000 | *c++;

	sprintf(buffer, "%5u", game->lines);
	here = &MAP1[(18 * 64) + 22];
	c = buffer;
	while (*c) *here++ = 0x2000 | *c++;

	sprintf(buffer, "%5u", game->level);
	here = &MAP1[(18 * 64) + 30];
	c = buffer;
	while (*c) *here++ = 0x2000 | *c++;

/*
	if (game->is_paused || game->is_over) {

		if (game->is_paused) {
			snprintf(game->scene->infos[6], LEN_INFO_LINE,
				 "PAUSE");
			snprintf(game->scene->infos[7], LEN_INFO_LINE,
				 "Press P to Unpause, E to End Game");
		} else {
			snprintf(game->scene->infos[6], LEN_INFO_LINE,
				 "GAME OVER");
			snprintf(game->scene->infos[7], LEN_INFO_LINE,
				 "Press N to New Game, Q to Quit");
		}
	} else {
		snprintf(game->scene->infos[6], LEN_INFO_LINE,
			 "\n");
		snprintf(game->scene->infos[7], LEN_INFO_LINE,
			 "\n");
	}
*/
}

static action get_action(int delay)
{
	int in;
	delay /= 10;
	while (delay--)
	{
		vbl();
		in = REG_KEYIN;
		if (REG_JOYPAD & 1) in = KEY_UP;
		else if (REG_JOYPAD & 2) in = KEY_RIGHT;
		else if (REG_JOYPAD & 4) in = KEY_DOWN;
		else if (REG_JOYPAD & 8) in = KEY_LEFT;
		else if (REG_JOYPAD & 16) in = KEY_UP;
		rndseed += in;

		if (in)
		{
			int debounce = 10;
			while (debounce)
			{
				vbl();
				debounce--;
				if (REG_KEYIN == 0)
					break;
			}
		}

		switch (in)
		{
			case KEY_LEFT: return MOVE_LEFT;
			case KEY_RIGHT: return MOVE_RIGHT;
			case KEY_UP: return ROTATE;
			case KEY_DOWN: return MOVE_DOWN;
		}
	}
	return MOVE_DOWN;
	/*
	timeout(delay);

	switch (getch()) {
		case ' ':
			return DROP;
		case 'n':
			return NEW_GAME;
		case 'e':
			return END_GAME;
		case 'p':
			return PAUSE;
		case 'q':
			return QUIT;
		default:
			return MOVE_DOWN;
	}
	*/
}

static void push_in_history(action action, game *game)
{
	game->history_moves[game->history_index] = action;
	game->history_index = (game->history_index + 1) % HISTORY_SIZE;
}

static void pause_game(game *game)
{
	action action;

	game->is_paused = 1;
	update_infos(game);
	refresh_grid(game->grid);

	while ((action = get_action(-1)) != PAUSE && action != END_GAME)
		vbl();

	if (action == END_GAME)
		game->is_over = 1;
	game->is_paused = 0;
}

static int remove_full_lines(game *game)
{
	int lines = 0;

	for (int row = 0; row < game->grid->rows; row++)
	{
		if (is_full_row(game->grid, row))
		{
			lines++;
			flush_row(game->grid, row);
			for (int i = row; i > 0; i--)
				swap_rows(game->grid, i, i - 1);
		}
	}
	return lines;
}

static void increase_level(game *game)
{
	unsigned int level = (game->lines / 10) + 1;

	if (level <= game->level)
		return;

	game->level = level;

	if (game->delay > 200)
		game->delay -= 100;
	else if (game->delay > 150)
		game->delay -= 5;
}

static void increase_score(game *game, int lines)
{
	int m = 0;

	switch (lines)
	{
		case 1:
			m = 40;
			break;
		case 2:
			m = 100;
			break;
		case 3:
			m = 300;
			break;
		case 4:
			m = 1200;
			break;
		default:
			break;
	}
	game->score += m * (game->level + 1);
}

action game_over(game *game)
{
	action action;

	game->is_over = 1;

	update_infos(game);
	refresh_grid(game->grid);

	while ((action = get_action(-1)) != NEW_GAME && action != QUIT)
		vbl();

	if (action == NEW_GAME)
		reset_game(game);

	return action;
}

void free_game(game *game)
{
	free_grid(game->grid);
	free(game->falling);
	free(game);
}
