#include <stdlib.h>

#include "tinycols.h"

enum color random_color(void)
{
	return rand() % COLOR_MAX + 1;
}

struct grid *grid_alloc(uint8_t rows, uint8_t cols)
{
	const unsigned int size = rows * cols;
	struct grid *gr = malloc(sizeof *gr + sizeof(gr->cells[0]) * size);

	if (gr == NULL) {
		return NULL;
	}

	gr->cols = cols;
	gr->rows = rows;
	gr->size = size;

	return gr;
}

static inline enum color get_cell(struct grid *gr, int row, int col)
{
	return gr->cells[(row *  gr->cols) + col];
}

static inline void set_cell(struct grid *gr, int row, int col, enum color clr)
{
	gr->cells[(row * gr->cols) + col] = clr;
}

void grid_init(struct grid *gr)
{
	for (int i = 0; i < gr->size; i++) {
		gr->cells[i] = TRANSPARENT;
	}
}

static inline unsigned long calc_score(unsigned int jewel_count)
{
	return (jewel_count - PIECE_SIZE + 1) * SCORE_PER_PIECE;
}

score_t grid_scan(const struct grid *gr, uint8_t *result)
{
	unsigned int size = gr->size;
	score_t score = 0;

	for (int i = 0; i < size; i++) {
		result[i] = 0;
	}

	for (int idx = 0; idx < size; idx++) {
		const uint8_t color = gr->cells[idx];
		if (color != TRANSPARENT) {
			// Horizontal check
			unsigned int count = 1;
			unsigned int lim = ((idx / gr->cols) * gr->cols) + gr->cols;
			for (int i = 1, ldx; (ldx = idx + i) < lim; i++) {
				if (gr->cells[ldx] == color) {
					count++;
				} else break;
			}
			if (count >= PIECE_SIZE) {
				score += calc_score(count);
				for (int i = 0; i < count; i++) {
					result[idx + i] = color;
				}
			}
			// Vertical check
			count = 1;
			for (int i = 1, ldx; (ldx = idx + i * gr->cols) < size; i++) {
				if (gr->cells[ldx] == color) {
					count++;
				} else break;
			}
			if (count >= PIECE_SIZE) {
				score += calc_score(count);
				for (int i = 0; i < count; i++) {
					result[idx + (i * gr->cols)] = color;
				}
			}
			// Diagonal check: left, down
			count = 1;
			for (int i = 1, row = idx / gr->cols; i < size; i++) {
				const int ldx = idx - i + (i * gr->cols);
				if ((ldx / gr->cols == row + i) &&
					ldx < size &&
					(gr->cells[ldx] == color)) {
					count++;
				} else break;
			}
			if (count >= PIECE_SIZE) {
				score += calc_score(count);
				for (int i = 0; i < count; i++) {
					result[idx - i + (i * gr->cols)] = color;
				}
			}
			// Diagonal check: right, down
			count = 1;
			for (int i = 1, row = idx / gr->cols; i < size; i++) {
				const int ldx = idx + i * gr->cols + i;
				if ((ldx / gr->cols == row + i) &&
					ldx < size &&
					(gr->cells[ldx] == color)) {
					count++;
				} else break;
			}
			if (count >= PIECE_SIZE) {
				score += calc_score(count);
				for (int i = 0; i < count; i++) {
					result[idx + i + (i * gr->cols)] = color;
				}
			}
		}
	}

	return score;
}

uint16_t grid_remove_jewels(struct grid *gr, const uint8_t *jewels)
{
	uint16_t removed = 0;

	for (int i = 0; i < gr->size; i++) {
		if (jewels[i] != TRANSPARENT) {
			gr->cells[i] = TRANSPARENT;
			removed++;
		}
	}

	return removed;
}

static inline void init_drops(struct drop *drs, unsigned int max_drops)
{
	for (unsigned int i = 0; i < max_drops; i++) {
		(drs + i)->col = 0;
		(drs + i)->row = 0;
		(drs + i)->n = 0;
		(drs + i)->h = 0;
	}
}

unsigned int
grid_detect_drops(struct grid *gr, struct drop *drs, unsigned int max_drops)
{
	const int cols = gr->cols;
	const int rows = gr->rows;
	int n = 0;
	int n_drops = 0;

	init_drops(drs, max_drops);

	for (int i = 0; i < cols; i++) {
		int gap_end = NOT_FOUND;
		int block_end = NOT_FOUND;

		for (int l = 0; l < rows; l++) {
			const int idx = cols * (rows - l) - i - 1;
			const int cell = (gr->cells)[idx];
			if (cell == TRANSPARENT) {
				if (block_end != NOT_FOUND && gap_end != NOT_FOUND) {
					(drs + n)->col = cols - i - 1;
					(drs + n)->row = (idx + cols) / cols;
					(drs + n)->n = (block_end / cols) - (idx / cols);
					(drs + n)->h = (gap_end / cols) - (block_end / cols);
					n++;
					n_drops++;
					gap_end = NOT_FOUND;
					block_end = NOT_FOUND;
				}
				if (gap_end == NOT_FOUND) {
					gap_end = idx;
					block_end = NOT_FOUND;
				}
			} else if (block_end == NOT_FOUND) {
				block_end = idx;
			}
		}

		// Clean up column
		if (block_end != NOT_FOUND && gap_end != NOT_FOUND) {
			(drs + n)->col = cols - i - 1;
			(drs + n)->row = 0; // Always zero, as we are at the top
			(drs + n)->n = block_end / cols + 1;
			(drs + n)->h = (gap_end / cols) - (block_end / cols);
			n++;
			n_drops++;
		}
	}

	return n_drops;
}

void
grid_apply_drops(struct grid *gr, const struct drop *drs, unsigned int n_drops)
{
	for (int i = 0; i < n_drops; i++) {
		const struct drop *dr = drs + i;
		for (int l = dr->n - 1; l >= 0; l--) {
			const int start = (dr->row + l) * gr->cols + dr->col;
			gr->cells[start + dr->h * gr->cols] = gr->cells[start];
			gr->cells[start] = TRANSPARENT;
		}
	}
}

void piece_randomize(struct piece *pc)
{
	for (int i = 0; i < PIECE_SIZE; i++) {
		pc->colors[i] = random_color();
	}
}

bool piece_persist(struct piece *pc, struct grid *gr)
{
	if (pc->col < 0 || pc->row < 0 ||
		pc->col >= gr->cols || pc->row >= gr->rows ||
		pc->col + PIECE_SIZE > gr->rows) {
		return false;
	}

	for (int i = 0; i < PIECE_SIZE; i++) {
		set_cell(gr, pc->row + i, pc->col, pc->colors[i]);
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

static inline bool
is_blocked(struct piece *pc, enum direction d, struct grid *gr)
{
	int offset = (d == RIGHT) ? 1 : -1;
	for (int i = 0; i < PIECE_SIZE; i++) {
		int row = pc->row + i;
		if (row >= 0 && get_cell(gr, row, pc->col + offset) != TRANSPARENT) {
			return true;
		}
	}
	return false;
}

enum result
piece_move_in_grid(struct piece *pc, enum direction dir, struct grid *gr)
{
	switch (dir) {
		case DOWN: {
			if (pc->row >= gr->rows - PIECE_SIZE) {
				return (pc->status = LANDED);
			} else if (
				get_cell(gr, pc->row + PIECE_SIZE, pc->col) != TRANSPARENT) {
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

bool grid_position_piece(struct grid *gr, struct piece *pc)
{
	pc->col = gr->cols / 2;
	pc->row = 1 - PIECE_SIZE;
	if (get_cell(gr, pc->row + 1, pc->col) != TRANSPARENT ||
		get_cell(gr, pc->row + 2, pc->col) != TRANSPARENT) {
		return false;
	}

	return true;
}

struct game *game_alloc(void)
{
	struct grid *gr = grid_alloc(GRID_DEFAULT_ROWS, GRID_DEFAULT_COLS);
	if (gr == NULL) {
		return NULL;
	}

	struct game *gm = malloc(sizeof *gm);
	if (gm == NULL) {
		return NULL;
	}

	gm->grid = gr;
	return gm;
}

void game_cycle_piece(struct game *gm)
{
	for (int i = 0; i < PIECE_SIZE; i++) {
		gm->current_piece.colors[i] = gm->next_piece.colors[i];
	}
	piece_randomize(&gm->next_piece);
}

void game_init(struct game *gm, unsigned int level, enum color color_max)
{
	gm->score = 0;
	gm->level = level;
	gm->status = GAME_READY;
	gm->color_max = color_max;

	piece_randomize(&gm->next_piece);
	game_cycle_piece(gm);
	grid_init(gm->grid);
	grid_position_piece(gm->grid, &gm->current_piece);
}

void game_free(struct game *gm)
{
	free(gm->grid);
	free(gm);
}

enum color test_get_cell(struct grid *gr, int row, int col)
{
	return get_cell(gr, row, col);
}

void test_set_cell(struct grid *gr, int row, int col, enum color clr)
{
	set_cell(gr, row, col, clr);
}
