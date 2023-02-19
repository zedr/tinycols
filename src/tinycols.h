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

enum direction {
	UP,
	DOWN,
	LEFT,
	RIGHT
};

enum result {
	UNKNOWN,
	MOVED,
	BLOCKED,
	LANDED,
	PERSISTED
};

enum game_status {
	GAME_READY,
	GAME_ACTIVE,
	GAME_OVER
};

struct grid {
	int cols;
	int rows;
	int size;
	uint8_t cells[];
};

struct piece {
	int row;
	int col;
	enum result status;
	enum color colors[PIECE_SIZE];
};

struct drop {
	int col; // grid x location of the group to drop
	int row; // grid y location of the top of the group to drop
	int n; // number of blocks to drop (that form the group)
	int h; // height of the drop
};

struct game {
	unsigned short level;
	score_t score;
	score_t last_score;
	unsigned long jewels_removed;
	enum game_status status;
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
grid_detect_drops(struct grid *gr, struct drop *drs, unsigned int max_drops);

void
grid_apply_drops(struct grid *gr, const struct drop *drs, unsigned int n_drops);

bool grid_position_piece(struct grid *gr, struct piece *pc);

void piece_randomize(struct piece *pc);

bool piece_persist(struct piece *pc, struct grid *gr);

void piece_rotate(struct piece *pc, enum direction dir);

enum result
piece_move_in_grid(struct piece *pc, enum direction dir, struct grid *gr);

// Test functions
enum color test_get_cell(struct grid *gr, int row, int col);

void
test_set_cell(struct grid *gr, int row, int col, enum color clr);

//  Game functions
struct game *game_alloc(void);

void game_init(struct game *gm, unsigned int level, enum color color_max);

void game_cycle_piece(struct game *gm);

void game_free(struct game *gm);

#endif //TINYCOLS_H
