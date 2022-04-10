#include <stdlib.h>

#include "core.h"

static ushort_t
rand_jewel (ushort_t max)
{
	return (rand () % max) + 1;
}

void
init_piece (piece_t *piece, ushort_t j1, ushort_t j2, ushort_t j3)
{
	piece->x = 0;
	piece->y = 0;
	piece->jewels[0] = j1;
	piece->jewels[1] = j2;
	piece->jewels[2] = j3;
}

void
init_rand_piece (piece_t *piece, ushort_t difficulty)
{
	init_piece (piece,
				rand_jewel (difficulty),
				rand_jewel (difficulty),
				rand_jewel (difficulty));
}

void
init_board (board_t *board, board_t *source_board)
{
	if (source_board == NULL) {
		for (int i = 0; i < BOARD_CELLS; i++) {
			(*board)[i] = 0;
		}
	}
}

void
init_game (game_t *game, int difficulty)
{
	game->difficulty = difficulty;
	game->board = AllocMem (sizeof (*(game->board)), MEMF_PUBLIC);
	game->next = AllocMem (sizeof (*(game->next)), MEMF_PUBLIC);
	game->current = AllocMem (sizeof (*(game->current)), MEMF_PUBLIC);
	game->interval = DEFAULT_INTERVAL;

	init_board (game->board, NULL);
	init_rand_piece (game->next, difficulty);
}

void
free_game (game_t *game)
{
	FreeMem (game->board, sizeof (*game->board));
	FreeMem (game->next, sizeof (*game->next));
	FreeMem (game->current, sizeof (*game->current));
	FreeMem (game, sizeof (*game));
}

void
pick_next (game_t *game, piece_t *new_next)
{
	game->current->jewels[0] = game->next->jewels[0];
	game->current->jewels[1] = game->next->jewels[1];
	game->current->jewels[2] = game->next->jewels[2];
	if (new_next == NULL) {
		init_rand_piece (game->next, game->difficulty);
	}
	game->current->x = PIECE_START_X;
	game->current->y = PIECE_START_Y;
}

void
pick_and_position_next (game_t *game)
{
	pick_next (game, NULL);
	game->next->x = BOARD_COLS + 2;
	game->next->y = 0;
}

/*
 * place_piece()
 *
 * Returns:
 *  1 if the piece was correctly placed inside the board;
 *  0 if the piece was placed outside the top (game over)
 */
int
place_piece (piece_t *piece, board_t *board)
{
	for (int i = 0, y; i < PIECE_SIZE; i++) {
		y = piece->y + i;
		if (y < 0) {
			return 0;
		} else {
			(*board)[(y * BOARD_COLS) + piece->x] = piece->jewels[i];
		}
	}
	return 1;
}

void
shift_command (command_t cmd, piece_t *piece, void (*cb) (piece_t *piece))
{
	ushort_t tmp;
	ushort_t *jewels = piece->jewels;

	if (cmd == SHIFT_UP) {
		tmp = jewels[0];
		jewels[0] = jewels[1];
		jewels[1] = jewels[2];
		jewels[2] = tmp;
	} else if (cmd == SHIFT_DOWN) {
		tmp = jewels[2];
		jewels[2] = jewels[1];
		jewels[1] = jewels[0];
		jewels[0] = tmp;
	}

	if (cb) (*cb) (piece);
}

int
move_command (command_t cmd, game_t *game, void (*cb) (piece_t *piece))
{
	board_t *board = game->board;
	piece_t *cur = game->current;
	int px = cur->x;
	int py = cur->y;
	ushort_t r1, r2, r3, l1, l2, l3;

	switch (cmd) {
		case MOVE_RIGHT:
			if (px < (BOARD_COLS - 1)) {
				r1 = (py < 0) ? 0 : (*board)[(py * BOARD_COLS) + px + 1];
				r2 = (py + 1 < 0) ? 0 : (*board)[((py + 1) * BOARD_COLS) + px
												 + 1];
				r3 = (py + 2 < 0) ? 0 : (*board)[((py + 2) * BOARD_COLS) + px
												 + 1];
				if (!(r1 | r2 | r3)) {
					if (cb) (*cb) (cur);
					cur->x++;
					return 1;
				}
			}
			break;
		case MOVE_LEFT:
			if (px > 0) {
				l1 = (py < 0) ? 0 : (*board)[(py * BOARD_COLS) + px - 1];
				l2 = (py + 1 < 0) ? 0 : (*board)[((py + 1) * BOARD_COLS) + px
												 - 1];
				l3 = (py + 2 < 0) ? 0 : (*board)[((py + 2) * BOARD_COLS) + px
												 - 1];
				if (!(l1 | l2 | l3)) {
					if (cb) (*cb) (cur);
					cur->x--;
					return 1;
				}
			}
			break;
		case MOVE_DOWN:
			if ((py + PIECE_SIZE < BOARD_ROWS)
				&& (*board)[(py + PIECE_SIZE) * BOARD_COLS + px] == 0) {
				if (cb) (*cb) (cur);
				cur->y++;
				return 1;
			}
			break;
		default:
			break;
	}
	return 0;
}

ushort_t
inspect_board (board_t *board, ushort_t x, ushort_t y)
{
	return (*board)[y * BOARD_COLS + x];
}

int
delete_clusters (board_t *board, results_t *results)
{
	int count = 0;

	for (int i = 0; i < BOARD_CELLS; i++) {
		if (results->targets[i] > 0) {
			(*board)[i] = 0;
			count++;
		}
	}

	return count;
}

int
find_drops (board_t *board, drops_t *drops)
{
	int found = 0;

	ushort_t cv;
	for (int col = 0, eb, eg, acc; col < BOARD_COLS; col++) {
		eb = -1;
		eg = -1;
		acc = 0;
		for (int c = BOARD_CELLS - BOARD_COLS + col, tmp; c >= 0; c -= BOARD_COLS) {
			(*drops)[c][0] = 0;
			(*drops)[c][1] = 0;
			cv = (*board)[c];
			if (cv) {
				if (eg != -1 && eb == -1) {
					// end of block
					eb = c;
				}
			} else {
				if (eb != -1) {
					// start of block
					tmp = ((eg - eb) / BOARD_COLS);
					(*drops)[c + BOARD_COLS][0] = (eb - c) / BOARD_COLS;
					(*drops)[c + BOARD_COLS][1] = tmp + acc;
					acc += tmp;
					found++;
					eb = -1;
					eg = c;
				} else if (eg == -1){
					// end of gap
					eg = c;
				}
			}
		}
		if (eb != -1) {
			(*drops)[col][0] = (eb + BOARD_COLS) / BOARD_COLS;
			(*drops)[col][1] = (eg - eb) / BOARD_COLS + acc;
			found++;
		}
	}

	return found;
}

void
apply_gravity (board_t *board, drops_t *drops)
{
	int (*cell)[2];
	int gap = 0;
	int len = 0;


	for (int i = BOARD_CELLS - 1; i > -1; i--) {
		cell = &(*drops)[i];
		len = (*cell)[0];
		if (len != 0) {
			gap = (*cell)[1];
			for (int j = len - 1; j >= 0; j--) {
				(*board)[i + (gap + j) * BOARD_COLS] = (*board)[i + j * BOARD_COLS];
				(*board)[i + j * BOARD_COLS] = 0;
			}
		}
	}
}

void
init_results (results_t *results)
{
	for (int i = 0; i < MAX_DIFFICULTY + 1; i++) {
		results->clusters[i].count = 0;
	}
	for (int l = 0; l < BOARD_CELLS; l++) {
		results->targets[l] = 0;
	}
}

void
append (cluster_t *cls, ushort_t col, results_t *res)
{
	ushort_t n;
	cluster_t *tmp_cls = &res->clusters[col - 1];

	for (int i = 0; i < cls->count; i++) {
		n = cls->indexes[i];
		if (!res->targets[n]) {
			tmp_cls->indexes[tmp_cls->count++] = n;
			res->targets[n] = 1;
		}
	}
}

static void
search (ushort_t col, int idx, board_t *board, int d, results_t *results,
		cluster_t *cls)
{
	cluster_t *tmp_cls;
	int ldx = idx + d;

	if (cls == NULL) {
		cluster_t _cls;
		_cls.count = 0;
		tmp_cls = &_cls;
	} else {
		tmp_cls = cls;
	}
	tmp_cls->indexes[tmp_cls->count++] = idx;

	if (ldx >= 0 && ldx < BOARD_CELLS && (*board)[ldx] == col) {
		if (
			!((d == DIR_E) && ((idx / BOARD_COLS) != (ldx / BOARD_COLS))) &&
			!((d == DIR_SE) && ((idx / BOARD_COLS + 1) != (ldx / BOARD_COLS)))
			&&
			!((d == DIR_SW) && ((idx / BOARD_COLS + 1) != (ldx / BOARD_COLS)))
			) {
			search (col, ldx, board, d, results, tmp_cls);
		}
	}

	if (tmp_cls->count >= PIECE_SIZE) {
		append (tmp_cls, col, results);
		tmp_cls->count = 0;
	}
}

void
find_clusters (board_t *board, results_t *results)
{
	init_results (results);

	for (int i = 0, col; i < BOARD_CELLS; i++) {
		col = (*board)[i];
		if (col) { ;
			search (col, i, board, DIR_S, results, NULL);
			search (col, i, board, DIR_E, results, NULL);
			search (col, i, board, DIR_SE, results, NULL);
			search (col, i, board, DIR_SW, results, NULL);
		}
	}
}
