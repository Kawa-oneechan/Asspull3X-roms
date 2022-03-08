#include "../ass.h"
#include "game.h"
extern uint32_t rndseed;
extern void srand(uint32_t seed);
extern uint32_t rand();

#define sprintf TEXT->Format

void Write(int x, int y, int p, const char* buffer)
{
	uint16_t *here = &MAP1[(y * 64) + x];
	char *c = (char*)buffer;
	p <<= 12;
	while (*c) *here++ = p | *c++;
}

extern int32_t vsprintf(char*, const char*, va_list);
void WriteF(int x, int y, int p, const char* format, ...)
{
	char buffer[256];
	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);
	Write(x, y, p, buffer);
}


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
	srand(REG_TIMET);

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

	WriteF(28, 15, 2, "%7u", game->score);
	WriteF(22, 18, 2, "%5u", game->lines);
	WriteF(30, 18, 2, "%5u", game->level);
	//else if (game->is_over)
		//snprintf(game->scene->infos[6], LEN_INFO_LINE, "GAME OVER");
	/*
	else
	{
		snprintf(game->scene->infos[6], LEN_INFO_LINE,
			 "\n");
		snprintf(game->scene->infos[7], LEN_INFO_LINE,
			 "\n");
	}
	*/
}

static action get_action(int delay)
{
	delay /= 10;
	while (delay--)
	{
		vbl();
		REG_JOYPAD = 1; //reset
		int dpadbuts = REG_JOYPAD;
		int extrabuts = REG_JOYPAD;
		rndseed += dpadbuts + extrabuts;

		if (dpadbuts)
		{
			int debounce = 10;
			if (dpadbuts & (1 | 16)) debounce = 20;
			if (dpadbuts & 32) debounce = 20;
			while (debounce)
			{
				vbl();
				debounce--;
				REG_JOYPAD = 1; //reset
				if (REG_JOYPAD == 0)
					break;
			}
		}

		if (dpadbuts & 1) return ROTATE;
		if (dpadbuts & 2) return MOVE_RIGHT;
		if (dpadbuts & 8) return MOVE_LEFT;
		if (dpadbuts & 4) return MOVE_DOWN;
		if (dpadbuts & 16) return ROTATE;
		if (dpadbuts & 32) return DROP;
		if (extrabuts & 8) return PAUSE;
	}
	return MOVE_DOWN;
}

static void push_in_history(action action, game *game)
{
	game->history_moves[game->history_index] = action;
	game->history_index = (game->history_index + 1) % HISTORY_SIZE;
}

static void pause_game(game *game)
{
	game->is_paused = 1;
	REG_SCREENFADE = 4;
	Write(22, 15, 0, "    PAUSE    ");
	refresh_grid(game->grid);

	//debounce
	while (1)
	{
		vbl();
		REG_JOYPAD = 1;
		int buttons = REG_JOYPAD;
		buttons = REG_JOYPAD;
		if (!buttons) break;
	}
	while (1)
	{
		vbl();
		REG_JOYPAD = 1;
		int buttons = REG_JOYPAD;
		buttons = REG_JOYPAD;
		if (buttons & 8) break;
	}
	//debounce
	while (1)
	{
		vbl();
		REG_JOYPAD = 1;
		int buttons = REG_JOYPAD;
		buttons = REG_JOYPAD;
		if (!buttons) break;
	}

	REG_SCREENFADE = 0;
	Write(22, 15, 1, "SCORE ");
	game->is_paused = 0;
}

static int remove_full_lines(game *game)
{
	int lines = 0;

	for (int row = 0; row < game->grid->rows; row++)
	{
		if (is_full_row(game->grid, row))
		{
			refresh_grid(game->grid);
			lines++;

			//Animate!
			for (int col = 0; col < game->grid->cols; col++)
			{
				MAP2[(row * 64) + col] = 0x101C;
				vbl();
				vbl();
				vbl();
				vbl();
				MAP2[(row * 64) + col] = 0;
			}

			flush_row(game->grid, row);
			for (int i = row; i > 0; i--)
				swap_rows(game->grid, i, i - 1);
		}
	}
	return lines;
}

static void increase_level(game *game)
{
	uint32_t level = (game->lines / 10) + 1;

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
