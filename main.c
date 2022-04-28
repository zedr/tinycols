#include "main.h"

#include <stdlib.h>
#include <ncurses.h>
#include <time.h>
#include <unistd.h>

#include "core.h"

WINDOW *win;
int start_x = 0, start_y = 0, debug = 0;
unsigned long last_ts = 0;

void draw_rect (int x1, int y1, int x2, int y2)
{
	mvhline(y1, x1, 0, x2 - x1);
	mvhline(y2, x1, 0, x2 - x1);
	mvvline(y1, x1, 0, y2 - y1);
	mvvline(y1, x2, 0, y2 - y1);
	mvaddch(y1, x1, ACS_ULCORNER);
	mvaddch(y2, x1, ACS_LLCORNER);
	mvaddch(y1, x2, ACS_URCORNER);
	mvaddch(y2, x2, ACS_LRCORNER);
}

static void
setup_screen (void)
{
	int win_w, win_h;

	win = initscr ();
	cbreak ();
	noecho ();
	clear();
	keypad (win, true);
	nodelay (win, true);
	curs_set (0);

	getmaxyx(win, win_h, win_w);
	start_x = (win_w / 2) - (BOARD_COLS / 2);
	start_y = (win_h / 2) - (BOARD_ROWS / 2);
}

static void
teardown_screen (void)
{
	endwin ();
}

static void
erase_piece (piece_t *piece)
{
	for (int i = 0; i < PIECE_SIZE; i++) {
		if (i + piece->y >= 0)
			mvaddch(piece->y + i + start_y,
					piece->x + start_x,
					' ');
	}
}

static void
draw_piece (piece_t *piece)
{
	for (int i = 0; i < PIECE_SIZE; i++) {
		if (i + piece->y >= 0)
			mvaddch(piece->y + i + start_y,
					piece->x + start_x,
					piece->jewels[i] + '0');
	}
}

static void
draw_frames (game_t *game)
{
	if (win != NULL) {
		draw_rect (start_x - 1,
				   start_y - 1,
				   start_x + BOARD_COLS,
				   start_y + BOARD_ROWS);
		draw_rect (start_x + BOARD_COLS + 1,
				   start_y - 1,
				   start_x + BOARD_COLS + 3,
				   start_y + PIECE_SIZE);
		draw_rect (start_x + BOARD_COLS + 1,
				   start_y + 4,
				   start_x + BOARD_COLS + 9,
				   start_y + 6);
	}
	refresh();
}

static void
draw_board (board_t *board)
{
	int x, y;
	for (int i = 0; i < BOARD_CELLS; i++) {
		ushort_t cell = (*board)[i];
		y = start_y + (i / BOARD_COLS);
		x = start_x + (i % BOARD_COLS);
		mvaddch(y, x, (cell) ? cell + '0' : ' ');
	}
}

static void
process_keys (game_t *game)
{
	switch (wgetch (win)) {
		case KEY_RIGHT:
			move_command (MOVE_RIGHT, game, &erase_piece);
			break;
		case KEY_LEFT:
			move_command (MOVE_LEFT, game, &erase_piece);
			break;
		case KEY_DOWN:
			if (move_command (MOVE_DOWN, game, &erase_piece)) {
				game->score += (game->level + 1);
			}
			break;
		case KEY_UP:
			shift_command (SHIFT_UP, game->current, &erase_piece);
			break;
		default:
			break;
	}
}

static void
display_deleted (results_t *results)
{
	int do_pause = 0;
	div_t d;

	for (int i = 0; i < BOARD_CELLS; i++) {
		if (results->targets[i] > 0) {
			do_pause = 1;
			d = div (i, BOARD_COLS);
			mvaddch(start_y + d.quot,
					start_x + d.rem,
					'*');
		}
	}

	if (do_pause) {
		refresh();
		usleep (250000);
	}
}

static void
display_points (unsigned long points)
{
	if (points) {
		mvprintw (start_y + 5,
				  start_x + 8,
				  "+%d",
				  points);
	}

}
static void
hide_points (void)
{
	mvprintw (start_y + 5,
			  start_x + 8,
			  "     ");
}

static int
game_tick (game_t *game)
{
	unsigned long ts = time (NULL);
	results_t results;
	int chain = 0;
	unsigned long points;
	int deleted;

	if (ts > last_ts) {
		last_ts = ts;
		if (!(move_command (MOVE_DOWN, game, &erase_piece))) {
			if (game->current->y > -1) {
				place_piece (game->current, game->board);
				init_results (&results);
				do {
					find_clusters (game->board, &results);
					points = calc_points (&results, game->level, chain);
					display_points (points);
					display_deleted (&results);
					hide_points ();
					game->score += points;
					deleted = delete_clusters (game->board, &results);
					if (deleted) {
						apply_gravity (game->board, &move_column);
						chain++;
					} else {
						break;
					}
				} while (TRUE);
				pick_and_position_next (game);
				draw_piece (game->next);
			} else {
				return 0;
			}
		}
	}

	usleep (100);

	return 1;
}

static void
draw_debug (game_t *game)
{
	mvprintw (1, 1, "score: \t%ld ", game->score);
	if (debug) {
		mvprintw (2, 1, "piece coords: \t%d, %d ",
				  game->current->x, game->current->y);
		mvprintw (3, 1, "st: \t%d, %d ", start_x, start_y);
	}
}

static void
play_game (void)
{
	game_t *game = AllocMem(sizeof (*game), 0);

	init_game (game, MAX_DIFFICULTY);
	pick_and_position_next (game);
	draw_frames (game);

	do {
		draw_board (game->board);
		draw_piece (game->current);
		draw_piece (game->next);
		draw_debug (game);
		process_keys (game);
		refresh();
	} while (game_tick (game));

	free_game (game);
}

int
main (int argc, char *argv[])
{
	srand ((uint32_t) time (NULL));

	setup_screen ();
	play_game ();
	teardown_screen ();
	return 0;
}
