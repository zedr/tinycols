#include <stdlib.h>
#include <stdint.h>

#include "../../include/tinycols/color.h"
#include "../../include/tinycols/game_utils.h"

struct grid *grid_alloc(uint8_t rows, uint8_t cols)
{
	const int size = (int)rows * cols;
	struct grid *gr = malloc(sizeof *gr + sizeof(gr->cells[0]) * size);

	if (gr == NULL) {
		return NULL;
	}

	gr->cols = cols;
	gr->rows = rows;
	gr->size = size;

	return gr;
}

void grid_init(struct grid *gr)
{
	for (int i = 0; i < gr->size; i++) {
		gr->cells[i] = TRANSPARENT;
	}
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

unsigned int grid_detect_drops(const struct grid *gr, struct grid_drop *drs,
			       unsigned int max_drops)
{
	int n = 0;
	int n_drops = 0;
	const int cols = gr->cols;
	const int rows = gr->rows;

	init_grid_drops(drs, max_drops);

	for (int i = 0; i < cols; i++) {
		int gap_end = NOT_FOUND;
		int block_end = NOT_FOUND;

		for (int l = 0; l < rows; l++) {
			const int idx = cols * (rows - l) - i - 1;
			const int cell = (gr->cells)[idx];
			if (cell == TRANSPARENT) {
				if (block_end != NOT_FOUND &&
				    gap_end != NOT_FOUND) {
					(drs + n)->col = cols - i - 1;
					(drs + n)->row = (idx + cols) / cols;
					(drs + n)->n = (block_end / cols) -
						       (idx / cols);
					(drs + n)->h = (gap_end / cols) -
						       (block_end / cols);
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
			(drs + n)->row = 0; // Always zero, as we are at the
				// top
			(drs + n)->n = block_end / cols + 1;
			(drs + n)->h = (gap_end / cols) - (block_end / cols);
			n++;
			n_drops++;
		}
	}

	return n_drops;
}

inline void init_grid_drops(struct grid_drop *drs, unsigned int max_drops)
{
	for (unsigned int i = 0; i < max_drops; i++) {
		(drs + i)->col = 0;
		(drs + i)->row = 0;
		(drs + i)->n = 0;
		(drs + i)->h = 0;
	}
}

void grid_apply_drops(struct grid *gr, const struct grid_drop *drs,
		      unsigned int n_drops)
{
	for (int i = 0; i < n_drops; i++) {
		const struct grid_drop *dr = drs + i;
		for (int l = dr->n - 1; l >= 0; l--) {
			const int start = (dr->row + l) * gr->cols + dr->col;
			gr->cells[start + dr->h * gr->cols] = gr->cells[start];
			gr->cells[start] = TRANSPARENT;
		}
	}
}

game_score_t grid_scan(const struct grid *gr, uint8_t *result)
{
	unsigned int size = gr->size;
	game_score_t score = 0;

	// Initialize the result to remove any previous information.
	for (int i = 0; i < size; i++) {
		result[i] = 0;
	}

	for (int idx = 0; idx < size; idx++) {
		const uint8_t color = gr->cells[idx];
		if (color != TRANSPARENT) {
			// Horizontal check
			unsigned int count = 1;
			unsigned int lim =
				((idx / gr->cols) * gr->cols) + gr->cols;
			for (int i = 1, ldx; (ldx = idx + i) < lim; i++) {
				if (gr->cells[ldx] == color) {
					count++;
				} else
					break;
			}
			if (count >= PIECE_SIZE) {
				score += calc_score(count);
				for (int i = 0; i < count; i++) {
					result[idx + i] = color;
				}
			}
			// Vertical check
			count = 1;
			for (int i = 1, ldx; (ldx = idx + i * gr->cols) < size;
			     i++) {
				if (gr->cells[ldx] == color) {
					count++;
				} else
					break;
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
				if ((ldx / gr->cols == row + i) && ldx < size &&
				    (gr->cells[ldx] == color)) {
					count++;
				} else
					break;
			}
			if (count >= PIECE_SIZE) {
				score += calc_score(count);
				for (int i = 0; i < count; i++) {
					result[idx - i + (i * gr->cols)] =
						color;
				}
			}
			// Diagonal check: right, down
			count = 1;
			for (int i = 1, row = idx / gr->cols; i < size; i++) {
				const int ldx = idx + i * gr->cols + i;
				if ((ldx / gr->cols == row + i) && ldx < size &&
				    (gr->cells[ldx] == color)) {
					count++;
				} else
					break;
			}
			if (count >= PIECE_SIZE) {
				score += calc_score(count);
				for (int i = 0; i < count; i++) {
					result[idx + i + (i * gr->cols)] =
						color;
				}
			}
		}
	}

	return score;
}