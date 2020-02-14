#include "../ass.h"
#include "drawer.h"

/*
 * draw_infos:  writes scene->infos strings starting from the top right corner
 *	      of the grid.
 */
static void draw_infos(const scene *scene);

/*
 * clear_infos:  remove scene->infos strings from the screen.
 */
static void clear_infos(const scene *scene);

/*
 * draw_grid:  draw scene->grid. Only draw blocks which have changed.
 */
static void draw_grid(const scene *scene);

/*
 * draw_block_if_different:  gets the current block in row, col from ncurses and
 *			   the future row, col block from scene->grid and
 *			   compares them: if they are equal skip the draw,
 *			   else draws the future block.
 */
static void draw_block_if_different(const scene *scene, int row, int col);


scene *init_scene(grid *grid,  char **infos, int infos_count)
{
	scene *new_scene = safe_malloc(sizeof(*new_scene));

	new_scene->grid = grid;
	new_scene->infos = infos;
	new_scene->infos_count = infos_count;
	return new_scene;
}

void refresh_scene(const scene *scene)
{
	clear_infos(scene);
	draw_infos(scene);
	draw_grid(scene);
}

static void draw_infos(const scene *scene)
{
	for (int row = 0; row < scene->infos_count; row++) {
	}
}

static void clear_infos(const scene *scene)
{
	for (int row = 0; row < scene->infos_count; row++) {
	}
}

static void draw_grid(const scene *scene)
{
	for (int row = 0; row < scene->grid->rows; row++)
		for (int col = 0; col < scene->grid->cols; col++)
			draw_block_if_different(scene, row, col);
}

static void draw_block_if_different(const scene *scene, int row, int col)
{
	short future_block; //current_block;

	//current_block = MAP2[(row * 64) + col];
	future_block = get_block(scene->grid, row, col);

	//if (future_block == current_block)
	//	return;
	//else if (is_empty_position(scene->grid, row, col))
	//	MAP2[(row * 64) + col] = 0;
	//else
		MAP2[(row * 64) + col] = future_block;
}

void free_scene(scene *scene)
{
	free(scene);
}
