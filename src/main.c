#include <ncurses.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "tinycols.h"
#include "gfx.h"

#define TICK_TIME 10000

WINDOW *win;
uint8_t tmp_res[GRID_DEFAULT_COLS * GRID_DEFAULT_ROWS];
struct drop tmp_drs[GRID_DEFAULT_COLS * GRID_DEFAULT_ROWS];

void process_keys(struct game *gm)
{
	switch (wgetch(win)) {
		case KEY_RIGHT:
			piece_move_in_grid(&gm->current_piece, RIGHT, gm->grid);
			break;
		case KEY_LEFT:
			piece_move_in_grid(&gm->current_piece, LEFT, gm->grid);
			break;
		case KEY_DOWN:
			piece_move_in_grid(&gm->current_piece, DOWN, gm->grid);
			break;
		case KEY_UP:
			piece_rotate(&gm->current_piece, UP);
			break;
		default:
			break;
	}

	flushinp();
}

/*
static unsigned int get_piece_time(struct game *gm)
{
	return MAX_TIMER / (gm->level + 1);
}
 */

static void game_tick(struct game *gm)
{
	if (gm->current_piece.status == PERSISTED) {
		grid_remove_jewels(gm->grid, tmp_res);
		unsigned int count = 0;
		while ((count = grid_detect_drops(gm->grid, tmp_drs, gm->grid->size))) {
			grid_apply_drops(gm->grid, tmp_drs, count);
		}
	}

	// Move the piece down
	if (gm->current_piece.status < LANDED) {
		piece_move_in_grid(&gm->current_piece, DOWN, gm->grid);
	}

	// Persist if landed
	if (gm->current_piece.status == LANDED) {
		if (piece_persist(&gm->current_piece, gm->grid)) {
			gm->current_piece.status = PERSISTED;
		} else {
			gm->status = GAME_OVER;
		}
	}

	// Scan the playing field for matches
	if (gm->current_piece.status == PERSISTED) {
		score_t tmp_score = 0;
		if ((tmp_score = grid_scan(gm->grid, tmp_res)) > 0) {
			gm->score += tmp_score;
		} else {
			game_cycle_piece(gm);
			grid_position_piece(gm->grid, &gm->current_piece);
			gm->current_piece.status = UNKNOWN;
		}
	}
}

static void run(void)
{
	win = setup_gfx();

	struct game *gm = game_alloc();
	if (gm == NULL) {
		perror("Out of memory");
		exit(EXIT_FAILURE);
	}
	game_init(gm, GAME_DEFAULT_LEVEL, GAME_DEFAULT_COLOR_MAX);

	draw_frame(gm, 0, 0);

	struct timeval time_start, time_end;
	uint16_t timer = 0;
	while (gm->status != GAME_OVER) {
		// Time Start
		gettimeofday(&time_start, NULL);

		// Process input
		if (gm->current_piece.status < LANDED) {
			process_keys(gm);
		}

		// Process game logic
		if (timer % 180 == 0) {
			game_tick(gm);
		}

		// Render game
		draw_frame(gm, 0, 0);
		draw_game(gm, 0, 0);
		if (gm->current_piece.status == PERSISTED) {
			draw_stars(tmp_res, gm->grid, 0, 0);
		} else {
			draw_piece(&gm->current_piece, 1, 1);
		}
		draw_debug(gm, 10, 10);
		refresh();

		// Time End
		gettimeofday(&time_end, NULL);
		usleep(time_start.tv_usec + TICK_TIME - time_end.tv_usec);
		timer++;
	}

	usleep(1000000);

	game_free(gm);
	teardown_gfx(win);
}

int main(int argc, char **argv)
{
	struct timeval time_now;
	gettimeofday(&time_now, NULL);
	srand((unsigned int) time_now.tv_usec); // NOLINT(cert-msc51-cpp)
	run();
	return 0;
}
