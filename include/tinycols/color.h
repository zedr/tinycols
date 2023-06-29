#ifndef TINYCOLS_COLOR_H
#define TINYCOLS_COLOR_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "grid.h"

#define COLOR_MAX YELLOW

/**
 * color - All the colors of a jewel piece.
 */
enum color {
	TRANSPARENT,
	PURPLE,
	ORANGE,
	BLUE,
	RED,
	GREEN,
	YELLOW
};

/**
 * get_cell_color() - Get the grid cell color at the given row and column.
 *
 * @param gr: The grid to inspect.
 * @param row: The row of the cell.
 * @param col: The column of the cell.
 * @return The color of the given cell.
 */
enum color get_cell_color(const struct grid *gr, int row, int col);

/**
 * set_cell_color() - Set the grid cell color at the given row and column.
 *
 * @param gr: The grid to inspect.
 * @param row: The row of the cell.
 * @param col: The column of the cell.
 * @param clr: The color to set.
 */
void set_cell_color(struct grid *gr, int row, int col, enum color clr);

#endif //TINYCOLS_COLOR_H
