#include "main.h"

#include <stdlib.h>
#include <ncurses.h>
#include <time.h>
#include <unistd.h>

#include "core.h"

WINDOW *win;
int start_x = 0, start_y = 0;
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
			move_command (MOVE_DOWN, game, &erase_piece);
			break;
		case KEY_UP:
			shift_command (SHIFT_UP, game->current, &erase_piece);
			break;
		default:
			break;
	}
}

static int
game_tick (game_t *game)
{
	unsigned long ts = time (NULL);
	results_t results;
	int deleted;

	if (ts > last_ts) {
		last_ts = ts;
		if (!(move_command (MOVE_DOWN, game, &erase_piece))) {
			if (game->current->y > -1) {
				place_piece (game->current, game->board);
				init_results (&results);
				do {
					find_clusters (game->board, &results);
					deleted = delete_clusters (game->board, &results);
					apply_gravity (game->board, NULL);
				} while (deleted);
				pick_and_position_next (game);
				draw_piece (game->next);
			} else {
				return 0;
			}
		}
	}

	usleep(100);

	return 1;
}

static void
draw_coords (piece_t *piece)
{
	mvprintw (1, 1, "piece coords: \t%d, %d ", piece->x, piece->y);
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
		draw_coords (game->current);
		process_keys (game);
		refresh();
		erase_piece (game->current);
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
