#include "../../include/tinycols/color.h"

inline enum color get_cell_color(const struct grid *gr, int row, int col)
{
	if ((row >= 0) && (row <= gr->rows) && (col <= gr->cols)) {
		return gr->cells[(row * gr->cols) + col];
	} else {
		return TRANSPARENT;
	}
}

inline void set_cell_color(struct grid *gr, int row, int col, enum color clr)
{
	if ((row >= 0) && (row <= gr->rows) && (col <= gr->cols)) {
		gr->cells[(row * gr->cols) + col] = clr;
	}
}
