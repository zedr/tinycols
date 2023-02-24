#include "gfx.h"

WINDOW *setup_gfx(void)
{
	WINDOW *win = initscr();
	if (win == NULL) {
		perror("Couldn't create a window");
		exit(EXIT_FAILURE);
	}
	cbreak();
	noecho();
	clear();
	keypad(win, true);
	nodelay(win, true);
	curs_set(0);
	return win;
}

void teardown_gfx()
{
	endwin();
}

static void draw_rect(int x1, int y1, int x2, int y2)
{
	mvhline(y1, x1, 0, x2 - x1);
	mvhline(y2, x1, 0, x2 - x1);
	mvvline(y1, x1, 0, y2 - y1);
	mvvline(y1, x2, 0, y2 - y1);
	mvaddch(y1, x1, ACS_ULCORNER);
	mvaddch(y2, x1, ACS_LLCORNER);
	mvaddch(y1, x2, ACS_URCORNER);
	mvaddch(y2, x2, ACS_LRCORNER);
	;
}

void draw_grid_jewels(const struct grid *gr, int x, int y)
{
	for (int i = 0; i < gr->size; i++) {
		div_t d = div(i, gr->cols);
		int jwl = gr->cells[i];
		mvaddch(y + d.quot, x + d.rem, (jwl) ? jwl + '0' : ' ');
	}
}

void draw_grid(struct grid *gr, int offset_x, int offset_y)
{
	draw_rect(offset_x, offset_y, gr->cols + 1, gr->rows + 1);
}

void draw_piece(struct piece *pc, int offset_x, int offset_y)
{
	for (int i = 0; i < PIECE_SIZE; i++) {
		int y0 = pc->row + i;
		if (y0 >= -1) {
			mvaddch(offset_y + y0, offset_x + pc->col,
				pc->colors[i] + '0');
		}
	}
}

void draw_frame(struct game *gm, int offset_x, int offset_y)
{
	int x = offset_x + gm->grid->cols + 2;
	draw_grid(gm->grid, offset_x, offset_y);
	draw_rect(x, offset_y, x + 2, offset_y + 4);
}

void draw_game(struct game *gm, int offset_x, int offset_y)
{
	struct grid *gr = gm->grid;

	draw_grid_jewels(gr, offset_x + 1, offset_y + 1);
	draw_piece(&gm->next_piece, offset_x + gr->cols + 3, offset_y + 1);
}

void draw_stars(const uint8_t *result, struct grid *gr, int x, int y)
{
	for (int i = 0; i < gr->size; i++) {
		div_t d = div(i, gr->cols);
		int clr = result[i];
		if (clr) {
			mvaddch(y + d.quot + 1, x + d.rem + 1, '*');
		}
	}
}

void draw_debug(struct game *g, int x, int y)
{
	mvprintw(y, x, "grid size:\t%d x %d ", g->grid->cols, g->grid->rows);
	mvprintw(y + 1, x, "piece coords:\t%d, %d ", g->current_piece.col,
		 g->current_piece.row);
	struct piece pc = g->current_piece;
	int idx = g->grid->cols * (pc.row + PIECE_SIZE) + pc.col;
	if (idx > 0 && idx < g->grid->cols * g->grid->rows) {
		mvprintw(y + 2, x, "under piece:\t%d ", g->grid->cells[idx]);
	}
	mvprintw(y + 3, x, "level:\t%hu ", (unsigned short)g->level);
	mvprintw(y + 4, x, "score:\t%lu ", (unsigned long)g->score);
	if (g->last_score > 0) {
		mvprintw(y + 5, x, "      \t+%lu ",
			 (unsigned long)g->last_score);
	} else {
		mvprintw(y + 5, x, "      \t     ");
	}
	mvprintw(y + 6, x, "jewels:\t%lu ", (unsigned long) g->jewels_removed);
}
