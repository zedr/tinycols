#ifndef _TINYCOLS_GFX_H_
#define _TINYCOLS_GFX_H_

#include <ncurses.h>
#include <stdlib.h>

#include "tinycols.h"

WINDOW *setup_gfx(void);

void teardown_gfx();

void draw_debug(struct game *g, int x, int y);

void draw_frame(struct game *g, int offset_x, int offset_y);

void draw_game(struct game *g, int offset_x, int offset_y);

void draw_piece(struct piece *p, int offset_x, int offset_y);

void draw_grid_jewels(const struct grid *gr, int x, int y);

void draw_stars(const uint8_t *result, struct grid *gr, int x, int y);

#endif // _TINYCOLS_GFX_H_
