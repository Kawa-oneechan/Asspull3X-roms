#include "../ass.h"
#include "drawer.h"

void refresh_grid(const grid *grid)
{
	for (int row = 0; row < grid->rows; row++)
		for (int col = 0; col < grid->cols; col++)
			MAP2[(row * 64) + col] = get_block(grid, row, col);
}
