#include "../ass.h"
#include "tetramino.h"

const char shapes_letters[TOT_SHAPES] = { 'I', 'J', 'L', 'O', 'S', 'T', 'Z' };

const char shapes[TOT_SHAPES][ROTATIONS][TETRAMINO_ROWS][TETRAMINO_COLS] =
{
	{	// I
		{
			{ 1, 1, 1, 1 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 0, 1, 0 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 1, 0 }
		},
		{
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 1, 1, 1, 1 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 1, 0, 0 }
		}
	},
	{	// J
		{
			{ 1, 0, 0, 0 },
			{ 1, 1, 1, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 1, 1, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 0, 0, 0 },
			{ 1, 1, 1, 0 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 0, 0, 0, 0 }
		}
	},
	{	// L
		{
			{ 0, 0, 1, 0 },
			{ 1, 1, 1, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 0, 0, 0 },
			{ 1, 1, 1, 0 },
			{ 1, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 1, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 0, 0 }
		}
	},
	{	// O
		{
			{ 1, 1, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 1, 1, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 1, 1, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 1, 1, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		}
	},
	{	// S
		{
			{ 0, 1, 1, 0 },
			{ 1, 1, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 1, 0, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 0, 0, 0 },
			{ 0, 1, 1, 0 },
			{ 1, 1, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 1, 0, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 0, 0 }
		}
	},
	{	// T
		{
			{ 0, 1, 0, 0 },
			{ 1, 1, 1, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 1, 0, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 0, 0, 0 },
			{ 1, 1, 1, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 1, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 0, 0 }
		}
	},
	{	// Z
		{
			{ 1, 1, 0, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 0, 1, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 0, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 1, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 1, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		}
	}
};


tetramino *create_tetramino(void)
{
	tetramino *new_tetramino = malloc(sizeof(*new_tetramino));
	change_shape(new_tetramino, 'I');
	return new_tetramino;
}

void change_shape(tetramino *tetramino, char shape_letter)
{
	int size, shape;

	switch (shape_letter)
	{
		case 'I':
			shape = 0;
			size = 4;
			break;
		case 'J':
			shape = 1;
			size = 3;
			break;
		case 'L':
			shape = 2;
			size = 3;
			break;
		case 'O':
			shape = 3;
			size = 2;
			break;
		case 'S':
			shape = 4;
			size = 3;
			break;
		case 'T':
			shape = 5;
			size = 3;
			break;
		case 'Z':
			shape = 6;
			size = 3;
			break;
		default:
			return;
	}
	tetramino->size = size;
	tetramino->rotation = 0;
	memcpy(tetramino->shape, shapes[shape], TETRAMINO_ROWS * TETRAMINO_COLS * ROTATIONS);
	tetramino->tile = 1 + shape;
	tetramino->on_grid = 0;
	tetramino->row = 0;
	tetramino->col = 0;
	tetramino->is_locked = 0;
}

void rotate_forward(tetramino *tetramino)
{
	tetramino->rotation = (tetramino->rotation + 1) % ROTATIONS;
}

void rotate_backwards(tetramino *tetramino)
{
	int back = tetramino->rotation - 1;
	tetramino->rotation = (back == -1) ? ROTATIONS - 1 : back;
}

void put_on_grid(grid *grid, tetramino *tetramino, int row, int col)
{
	if (tetramino->is_locked || tetramino->on_grid)
		return;

	for (int i = 0; i < tetramino->size; i++)
		for (int j = 0; j < tetramino->size; j++)
			if (tetramino->shape[tetramino->rotation][i][j])
				fill_block(grid, tetramino->tile, row + i, col + j);

	tetramino->on_grid = 1;
	tetramino->row = row;
	tetramino->col = col;
}

void remove_from_grid(grid *grid, tetramino *tetramino)
{
	if (tetramino->is_locked || ! tetramino->on_grid)
		return;

	for (int i = 0; i < tetramino->size; i++)
		for (int j = 0; j < tetramino->size; j++)
			if (tetramino->shape[tetramino->rotation][i][j])
				flush_block(grid, tetramino->row + i, tetramino->col + j);

	tetramino->on_grid = 0;
}
