#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "tinycols.h"
#include "gfx.h"

WINDOW *win;

enum result
process_keys(struct game *gm)
{
	enum result res = UNKNOWN;

	switch (wgetch(win)) {
		case KEY_RIGHT:
			res = piece_move_in_grid(&gm->current_piece, RIGHT, gm->grid);
			break;
		case KEY_LEFT:
			res = piece_move_in_grid(&gm->current_piece, LEFT, gm->grid);
			break;
		case KEY_DOWN:
			res = piece_move_in_grid(&gm->current_piece, DOWN, gm->grid);
			break;
		case KEY_UP:
			piece_rotate(&gm->current_piece, UP);
			break;
		default:
			break;
	}

	flushinp();
	return res;
}

static enum result game_tick(struct game *gm)
{
	return piece_move_in_grid(&gm->current_piece, DOWN, gm->grid);
}

static void run(void)
{
	struct game *gm = game_alloc();
	if (gm == NULL) {
		perror("Out of memory");
		exit(EXIT_FAILURE);
	}
	game_init(gm, GAME_DEFAULT_LEVEL, GAME_DEFAULT_COLOR_MAX);

	unsigned long ts = time(NULL);
	win = setup_gfx();

	uint8_t *tmp_res = calloc(gm->grid->size, sizeof *tmp_res);
	struct drop *drs = calloc(gm->grid->size, sizeof *tmp_res);

	while (gm->status != GAME_OVER) {
		enum result res = UNKNOWN;
		unsigned int count = 0;
		draw_game(gm, 0, 0);
		draw_debug(gm, 10, 10);
		if (time(NULL) > ts) {
			res = game_tick(gm);
			ts = time(NULL);
		}
		if (res == LANDED || process_keys(gm) == LANDED) {
			if (piece_persist(&gm->current_piece, gm->grid)) {
				score_t tmp;
				while ((tmp = grid_scan(gm->grid, tmp_res)) > 0) {
					gm->score += tmp;
					draw_stars(tmp_res,
							   1, 1,
							   gm->grid->cols, gm->grid->rows);
					grid_remove_jewels(gm->grid, tmp_res);
					while ((count = grid_detect_drops(gm->grid,
													  drs,
													  gm->grid->size)) > 0) {
						grid_apply_drops(gm->grid, drs, count);
					}
					refresh();
					usleep(250000);
					draw_jewels(gm->grid->cells,
								1, 1,
								gm->grid->cols, gm->grid->rows);
					refresh();
				}
				game_cycle_piece(gm);
				grid_position_piece(gm->grid, &gm->current_piece);
			} else {
				gm->status = GAME_OVER;
			}
		}
		flushinp();
		refresh();
		usleep(100);
	}
	usleep(1000000);
	teardown_gfx(win);
	game_free(gm);
	free(tmp_res);
	free(drs);
}

int main(int argc, char **argv)
{
	srand((uint32_t) time(NULL)); // NOLINT(cert-msc51-cpp)
	run();
	return 0;
}
