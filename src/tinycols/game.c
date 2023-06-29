#include <stdlib.h>

#include "../../include/tinycols/game.h"
#include "../../include/tinycols/game_utils.h"

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

void game_init(struct game *gm, unsigned int level, enum game_class class)
{
	gm->score = 0;
	gm->tick = 0;
	gm->level = level;
	gm->status = GAME_READY;
	gm->color_max = (enum color) class;

	piece_randomize(&gm->next_piece, gm->color_max);
	game_cycle_piece(gm);
	grid_init(gm->grid);
	position_piece_in_grid(&gm->current_piece, gm->grid);
	gm->current_piece.status = UNKNOWN;
	gm->next_piece.status = UNKNOWN;
}

bool game_adjust(struct game *gm)
{
	uint_least16_t jewels = gm->jewels_removed;
	unsigned short prev_level = gm->level;

	if (jewels < GAME_DEFAULT_JEWELS_FOR_LEVEL) {
		gm->level = 0;
		gm->color_max = GAME_DEFAULT_COLOR_MIN;
	} else if (jewels < GAME_DEFAULT_JEWELS_FOR_LEVEL * 2) {
		gm->level = 1;
		gm->color_max = GAME_DEFAULT_COLOR_MIN;
	} else if (jewels < GAME_DEFAULT_JEWELS_FOR_LEVEL * 3) {
		gm->level = 2;
		gm->color_max = GAME_DEFAULT_COLOR_MIN;
	}
	return prev_level != gm->level;
}

void game_cycle_piece(struct game *gm)
{
	for (int i = 0; i < PIECE_SIZE; i++) {
		gm->current_piece.colors[i] = gm->next_piece.colors[i];
	}
	piece_randomize(&gm->next_piece, gm->color_max);
}

void game_free(struct game *gm)
{
	free(gm->grid);
	free(gm);
}