#ifndef TINYCOLS_H
#define TINYCOLS_H

#include <stdint.h>
#include <stdbool.h>

#define GAME_DEFAULT_LEVEL 0
#define GAME_DEFAULT_COLOR_MAX YELLOW
#define GRID_DEFAULT_COLS 6
#define GRID_DEFAULT_ROWS 13
#define PIECE_SIZE 3
#define SCORE_PER_PIECE 30
#define NOT_FOUND -1

typedef uint_least16_t score_t;

/**
 * color - All the colors of a jewel piece.
 */
enum color {
	TRANSPARENT,
	PURPLE,
	ORANGE,
	BLUE,
	RED,
	GREEN,
	YELLOW
};

#define COLOR_MAX YELLOW

/**
 * direction - The direction a piece can take.
 */
enum direction {
	UP,
	DOWN,
	LEFT,
	RIGHT
};

/**
 * result - The result of an action affecting a piece.
 */
enum result {
	UNKNOWN,
	MOVED,
	BLOCKED,
	LANDED,
	PERSISTED
};

/**
 * game_status - The possible states a game can take.
 */
enum game_state {
	GAME_READY,
	GAME_OVER
};

/**
 * grid - The grid of a game.
 *
 * @param cols: The number of columns in this grid.
 * @param rows: The number of rows in this grid.
 * @param size: The size (number of cells) of this grid, i.e. rows * cols.
 * @param cells: The content of the cells of this grid.
 */
struct grid {
	int cols;
	int rows;
	int size;
	uint8_t cells[];
};

/**
 * piece - A jewel piece.
 *
 * @param row: The grid row where the top jewel of this piece is located.
 * @param col: The grid column where the top jewel of this piece is located.
 * @param status: The status of this piece, based on the result of the previous
 *                result of a movement.
 */
struct piece {
	int row;
	int col;
	enum result status;
	enum color colors[PIECE_SIZE];
};

/**
 * drop - A gap in the jewels that will cause the topmost to drop down
 *
 * @param row: The grid row the top of the group to drop.
 * @param col: The grid column the top of the group to drop.
 * @param n: The number of blocks to drop (that form the group).
 * @param h: The height of the drop.
 */
struct drop {
	int row;
	int col;
	int n;
	int h;
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
	score_t score;
	score_t last_score;
	uint_least16_t jewels_removed;
	enum game_state status;
	enum color color_max;
	struct piece current_piece;
	struct piece next_piece;
	struct grid *grid;
};

enum color random_color(void);

struct grid *grid_alloc(uint8_t rows, uint8_t cols);

void grid_init(struct grid *gr);

score_t grid_scan(const struct grid *gr, uint8_t *result);

uint16_t grid_remove_jewels(struct grid *gr, const uint8_t *jewels);

unsigned int
grid_detect_drops(const struct grid *gr, struct drop *drs,
				  unsigned int max_drops);

void
grid_apply_drops(struct grid *gr, const struct drop *drs, unsigned int n_drops);

bool grid_position_piece(struct grid *gr, struct piece *pc);

void piece_randomize(struct piece *pc);

bool piece_persist(struct piece *pc, struct grid *gr);

void piece_rotate(struct piece *pc, enum direction dir);

enum result
piece_move_in_grid(struct piece *pc, enum direction dir,
				   const struct grid *gr);

// Test functions
enum color test_get_cell(const struct grid *gr, int row, int col);

void
test_set_cell(struct grid *gr, int row, int col, enum color clr);

//  Game functions
struct game *game_alloc(void);

void game_init(struct game *gm, unsigned int level, enum color color_max);

void game_cycle_piece(struct game *gm);

void game_free(struct game *gm);

#endif //TINYCOLS_H
