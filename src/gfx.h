#ifndef _TINYCOLS_GFX_H_
#define _TINYCOLS_GFX_H_

#include <ncurses.h>
#include <stdlib.h>

#include "tinycols.h"

WINDOW *setup_gfx(void);

void teardown_gfx();

void draw_debug(struct game *g, int x, int y);

void draw_game(struct game *g, int offset_x, int offset_y);

void draw_piece(struct piece *p, int offset_x, int offset_y);

void draw_jewels(const uint8_t *jewels, int x, int y, int w, int h);

void draw_stars(const uint8_t *result, int x, int y, int w, int h);

#endif // _TINYCOLS_GFX_H_
