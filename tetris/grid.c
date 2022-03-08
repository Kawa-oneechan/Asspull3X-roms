#include "../ass.h"
#include "grid.h"


grid *create_grid(int rows, int cols)
{
	grid *new_grid = malloc(sizeof(*new_grid));
	int16_t *new_blocks = malloc(sizeof(*new_blocks) * rows * cols);

	new_grid->blocks = new_blocks;
	new_grid->rows = rows;
	new_grid->cols = cols;

	flush_grid(new_grid);

	return new_grid;
}

int is_valid_position(const grid *grid, int row, int col)
{
	return (row >= 0 && row < grid->rows) && (col >= 0 && col < grid->cols);
}

int is_empty_position(const grid *grid, int row, int col)
{
	return get_block(grid, row, col) == EMPTY_CONTENT;
}

int is_full_row(const grid *grid, int row)
{
	for (int col = 0; col < grid->cols; col++)
		if (is_empty_position(grid, row, col))
			return 0;
	return 1;
}

int16_t get_block(const grid *grid, int row, int col)
{
	return grid->blocks[row * grid->cols + col];
}

void fill_block(grid *grid, int16_t content, int row, int col)
{
	grid->blocks[row * grid->cols + col] = content;
}

void flush_block(grid *grid, int row, int col)
{
	grid->blocks[row * grid->cols + col] = EMPTY_CONTENT;
}

void flush_row(grid *grid, int row)
{
	for (int col = 0; col < grid->cols; col++)
		flush_block(grid, row, col);
}

void flush_grid(grid *grid)
{
	for (int row = 0; row < grid->rows; row++)
		flush_row(grid, row);
}

void swap_rows(grid *grid, int row1, int row2)
{
	int16_t temp1;
	int16_t temp2;

	for (int col = 0; col < grid->cols; col++)
	{
		temp1 = get_block(grid, row1, col);
		temp2 = get_block(grid, row2, col);
		fill_block(grid, temp1, row2, col);
		fill_block(grid, temp2, row1, col);
	}
}

void free_grid(grid *grid)
{
	free(grid->blocks);
	free(grid);
}
