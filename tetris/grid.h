#ifndef GRID_HEADER
#define GRID_HEADER

#include "helpers.h"

#define EMPTY_CONTENT 0

/*
 * grid:  represents a grid.
 *
 * blocks: a "dynamic multidimensional array" storing rows x cols blocks.
 * rows  : number of rows of the grid.
 * cols  : number of columns of the grid.
 */
typedef struct
{
        short *blocks;
        int rows;
        int cols;
} grid;

/*
 * create_grid:  creates and return (a pointer to) a new empty grid of size
 *               rows x cols.
 */
grid *create_grid(int rows, int cols);

/*
 * is_valid_position:  return true if row, col is a valid position on the grid
 *                     grid.
 */
int is_valid_position(const grid *grid, int row, int col);

/*
 * is_empty_position:  return true if the block at position row, col is empty.
 */
int is_empty_position(const grid *grid, int row, int col);

/*
 * is_full_row:  return true if every block in row is not empty.
 */
int is_full_row(const grid *grid, int row);

/*
 * get_block:  return a copy of the block at position row, col.
 */
short get_block(const grid *grid, int row, int col);

/*
 * fill_block:  puts content and color in the block at position row, col.
 */
void fill_block(grid *grid, short content, int row, int col);

/*
 * flush_block:  makes block at position row, col an empty block.
 */
void flush_block(grid *grid, int row, int col);

/*
 * flush_row:  makes all blocks in row empty blocks.
 */
void flush_row(grid *grid, int row);

/*
 * flush_grid:  makes all blocks in the grid grid empty blocks.
 */
void flush_grid(grid *grid);

/*
 * swap_rows:  swaps the blocks in row1 with the blocks in row2.
 */
void swap_rows(grid *grid, int row1, int row2);

/*
 * free_grid:  frees all memory occupied by grid.
 */
void free_grid(grid *grid);

#endif
