#include "../../include/tinycols/color.h"

inline enum color get_cell_color(const struct grid *gr, int row, int col)
{
	return gr->cells[(row * gr->cols) + col];
}

inline void set_cell_color(struct grid *gr, int row, int col, enum color clr)
{
	gr->cells[(row * gr->cols) + col] = clr;
}
