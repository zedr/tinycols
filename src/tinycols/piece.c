#include <stdint.h>
#include <stdlib.h>

#include "../../include/tinycols/piece.h"
#include "../../include/tinycols/direction.h"

/**
 * is_blocked() - Is the piece blocked from moving in a given direction?
 *
 * @param pc: The piece to inspect.
 * @param dir: The direction towards a movement is attempted.
 * @param gr: The grid to inspect.
 * @return Is the piece blocked from movement in that direction?
 */
static inline bool is_blocked(const struct piece *pc, enum direction dir,
			      const struct grid *gr)
{
	int offset = (dir == RIGHT) ? 1 : -1;
	for (int i = 0; i < PIECE_SIZE; i++) {
		int row = pc->row + i;
		if (row >= 0 &&
		    get_cell_color(gr, row, pc->col + offset) != TRANSPARENT) {
			return true;
		}
	}
	return false;
}

enum result piece_move_in_grid(struct piece *pc, enum direction dir,
			       const struct grid *gr)
{
	switch (dir) {
	case DOWN: {
		if (pc->row >= gr->rows - PIECE_SIZE) {
			return (pc->status = LANDED);
		} else if (get_cell_color(gr, pc->row + PIECE_SIZE, pc->col) !=
			   TRANSPARENT) {
			return (pc->status = LANDED);
		} else if (pc->row < gr->rows - 1) {
			pc->row++;
			return (pc->status = MOVED);
		}
		break;
	}
	case LEFT: {
		if (pc->col <= 0 || is_blocked(pc, LEFT, gr)) {
			break;
		} else {
			pc->col--;
			return (pc->status = MOVED);
		}
		break;
	}
	case RIGHT: {
		if (pc->col >= gr->cols - 1 || is_blocked(pc, RIGHT, gr)) {
			break;
		} else {
			pc->col++;
			return (pc->status = MOVED);
		}
		break;
	}
	default:
		break;
	}

	return (pc->status = BLOCKED);
}

static inline enum color random_color(unsigned int color_max)
{
	return rand() % color_max + 1;
}

void piece_randomize(struct piece *pc, unsigned int max_color)
{
	for (int i = 0; i < PIECE_SIZE; i++) {
		pc->colors[i] = random_color(max_color);
	}
}

bool piece_persist(struct piece *pc, struct grid *gr)
{
	if (pc->col < 0 || pc->row < 0 || pc->col >= gr->cols ||
	    pc->row >= gr->rows || pc->col + PIECE_SIZE > gr->rows) {
		return false;
	}

	for (int i = 0; i < PIECE_SIZE; i++) {
		set_cell_color(gr, pc->row + i, pc->col, pc->colors[i]);
	}

	pc->status = PERSISTED;

	return true;
}

void piece_rotate(struct piece *pc, enum direction dir)
{
	int tmp = (pc->colors)[2];
	if (dir == DOWN) {
		(pc->colors)[2] = (pc->colors)[1];
		(pc->colors)[1] = (pc->colors)[0];
		(pc->colors)[0] = tmp;
	} else if (dir == UP) {
		(pc->colors)[2] = (pc->colors)[0];
		(pc->colors)[0] = (pc->colors)[1];
		(pc->colors)[1] = tmp;
	}
}

bool position_piece_in_grid(struct piece *pc, const struct grid *gr)
{
	pc->col = gr->cols / 2;
	pc->row = 1 - PIECE_SIZE;
	if (get_cell_color(gr, pc->row + 1, pc->col) != TRANSPARENT ||
	    get_cell_color(gr, pc->row + 2, pc->col) != TRANSPARENT) {
		return false;
	}

	return true;
}
