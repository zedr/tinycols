#ifndef TINYCOLS_GAME_H
#define TINYCOLS_GAME_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "color.h"
#include "piece.h"
#include "score.h"

#define GAME_DEFAULT_LEVEL 0
#define GAME_DEFAULT_COLOR_MIN RED
#define GAME_DEFAULT_COLOR_MAX YELLOW
#define GAME_DEFAULT_JEWELS_FOR_LEVEL 30
#define GAME_SCORE_PER_PIECE 30

/**
 * game_status - The possible states a game can take.
 */
enum game_state { GAME_READY, GAME_PAUSED, GAME_OVER, GAME_EXIT };

/**
 * game_class - The game skill class, mapped to the appropriate colors
 */
enum game_class {
	CLASS_NOVICE = RED,
	CLASS_AMATEUR = GREEN,
	CLASS_PRO = YELLOW
};

/**
 * game - A game of Tiny Cols
 *
 * @param level: The current difficulty level of the game (starts from zero).
 * @param score: The current total score.
 * @param last_score: The last sub-score that was recorded in-game.
 * @param jewels_removed: The total number of jewels that were removed.
 * @param status: The current game status.
 * @param color_max: The maximum color that can be used in-game.
 *                   This is linked to the difficulty level.
 * @param current_piece: The current in-game piece, controlled by the player.
 * @param next_piece: The next piece to be played.
 * @param grid: The game grid.
 */
struct game {
	unsigned short level;
	uint8_t tick;
	game_score_t score;
	game_score_t last_score;
	uint_least16_t jewels_removed;
	enum game_state status;
	enum color color_max;
	struct piece current_piece;
	struct piece next_piece;
	struct grid *grid;
};

/**
 * game_alloc() - Allocate memory for a new game.
 *
 * @return The new grid, or NULL if memory allocation fails.
 */
struct game *game_alloc(uint8_t grid_rows, uint8_t grid_cols);

/**
 * game_init() - Initialize a game.
 *
 * @param gm: The game to initialize.
 * @param level: The level at which the game must be started.
 * @param class: The skill class of the game
 */
void game_init(struct game *gm, unsigned int level, enum game_class class);

/**
 * game_cycle_piece() - Cycle the current and next pieces in a given game.
 *
 * @param gm: The game to modify.
 */
void game_cycle_piece(struct game *gm);

/**
 * game_free() - Free the memory allocated for a game.
 *
 * @param gm: The game to free.
 */
void game_free(struct game *gm);
#endif //TINYCOLS_GAME_H
