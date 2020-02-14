#include "../ass.h"
#include "drawer.h"

scene *init_scene(grid *grid)
{
	scene *new_scene = malloc(sizeof(*new_scene));

	new_scene->grid = grid;
	return new_scene;
}

void refresh_scene(const scene *scene)
{
	for (int row = 0; row < scene->grid->rows; row++)
		for (int col = 0; col < scene->grid->cols; col++)
			MAP2[(row * 64) + col] = get_block(scene->grid, row, col);
}

void free_scene(scene *scene)
{
	free(scene);
}
