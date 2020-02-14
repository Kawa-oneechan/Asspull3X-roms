#include "controller.h"


int perform_action(grid *grid, tetramino *tetramino, action action)
{
        switch (action) {
                case ROTATE:
                        return rotate_tetramino(grid, tetramino);
                case MOVE_DOWN:
                case MOVE_LEFT:
                case MOVE_RIGHT:
                        return move_tetramino(grid, tetramino, action);
                case DROP:
                        return drop_tetramino(grid, tetramino);
                default:
                        return 0;
        }
}

int rotate_tetramino(grid *grid, tetramino *tetramino)
{
        int ret = 1;

        remove_from_grid(grid, tetramino);
        rotate_forward(tetramino);

        if ( ! check_position(grid, tetramino, tetramino->row, tetramino->col)) {
                rotate_backwards(tetramino);
                ret = 0;
        }
        put_on_grid(grid, tetramino, tetramino->row, tetramino->col);
        return ret;
}

int move_tetramino(grid *grid, tetramino *tetramino, action action)
{
        int future_row = tetramino->row;
        int future_col = tetramino->col;

        switch (action) {
                case MOVE_DOWN:
                        future_row += 1;
                        break;
                case MOVE_LEFT:
                        future_col -= 1;
                        break;
                case MOVE_RIGHT:
                        future_col += 1;
                        break;
                default:
                        return 0;
        }

        remove_from_grid(grid, tetramino);

        if ( ! check_position(grid, tetramino, future_row, future_col)) {

                put_on_grid(grid, tetramino, tetramino->row, tetramino->col);

                if (action == MOVE_DOWN) {
                        tetramino->is_locked = 1;
                        tetramino->on_grid = 0;
                }
                return 0;
        }
        put_on_grid(grid, tetramino, future_row, future_col);
        return 1;
}

int drop_tetramino(grid *grid, tetramino *tetramino)
{
        while ( ! tetramino->is_locked) {
                move_tetramino(grid, tetramino, MOVE_DOWN);
        }
        return 1;
}

int check_position(const grid *grid, const tetramino *tetramino,
                    int row, int col)
{
        for (int i = 0; i < tetramino->size; i++) {
                for (int j = 0; j < tetramino->size; j++) {

                        if (tetramino->shape[tetramino->rotation][i][j] &&
                           ( ! is_valid_position(grid, row + i, col + j) ||
                             ! is_empty_position(grid, row + i, col + j))) {
                                return 0;
                        }
                }
        }
        return 1;
}
