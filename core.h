#ifndef AMIGA_COLUMNS_CORE_H
#define AMIGA_COLUMNS_CORE_H

// Used for testing on Linux
#ifdef LINUX_TARGET
#include <stdlib.h>
#include <string.h>
#define AllocMem(byte_size, requirements) malloc(byte_size)
#define FreeMem(pointer, byte_size) free(pointer)
#define CopyMem(src_ptr, dst_ptr, size) memcpy(dst_ptr, src_ptr, size)
#define MEM_PUBLIC 0
#else
#include <clib/exec_protos.h>
#include <exec/memory.h>
#endif // LINUX_TARGET

#define DEFAULT_DIFFICULTY 4
#define MAX_DIFFICULTY 6

#define BOARD_COLS 6
#define BOARD_ROWS 13
#define BOARD_CELLS BOARD_COLS * BOARD_ROWS

#define MAX_CLUSTER_SIZE ((BOARD_ROWS > BOARD_COLS) > BOARD_ROWS : BOARD_COLS)

#define PIECE_SIZE 3
#define PIECE_START_X 2
#define PIECE_START_Y -2

#define DIR_S BOARD_COLS
#define DIR_E 1
#define DIR_SW (BOARD_COLS - 1)
#define DIR_SE (BOARD_COLS + 1)

#define DEFAULT_INTERVAL 500

#define MAX_CLUSTERS (BOARD_CELLS / PIECE_SIZE)

typedef unsigned short ushort_t;
typedef ushort_t board_t[BOARD_CELLS];
typedef ushort_t drops_t[BOARD_CELLS][2];

typedef enum {
	NONE,
	ORANGE,
	RED,
	YELLOW,
	BLUE,
	PURPLE,
	GREEN,
	MAGIC
} jewels_t;

typedef enum {
	NOOP,
	MOVE_LEFT,
	MOVE_RIGHT,
	MOVE_DOWN,
	SHIFT_UP,
	SHIFT_DOWN
} command_t;

typedef struct {
	ushort_t jewels[PIECE_SIZE];
	int x;
	int y;
} piece_t;

typedef struct {
	int difficulty;
	unsigned int interval;
	board_t *board;
	piece_t *next;
	piece_t *current;
} game_t;

typedef struct {
	int count;
	ushort_t indexes[BOARD_CELLS];
} cluster_t;

typedef struct {
	ushort_t targets[BOARD_CELLS];
	cluster_t clusters[MAX_DIFFICULTY + 1];
} results_t;

void
init_game (game_t *game, int difficulty);

void
init_board (board_t *board, board_t *source_board);

void
init_piece (piece_t *piece, ushort_t j1, ushort_t j2, ushort_t j3);

void
init_rand_piece (piece_t *piece, ushort_t difficulty);

void
pick_next (game_t *game, piece_t *new_next);

void
pick_and_position_next (game_t *game);

int
place_piece (piece_t *piece, board_t *board);

int
move_command (command_t cmd, game_t *game, void (*cb) (piece_t *piece));

void
shift_command (command_t cmd, piece_t *piece, void (*cb) (piece_t *piece));

ushort_t
inspect_board (board_t *board, ushort_t x, ushort_t y);

int
delete_clusters (board_t *board, results_t *results);

void
find_clusters (board_t *board, results_t *results);

int
find_drops(board_t *board, drops_t *drops);

void
apply_gravity(board_t *board, drops_t *drops);

void
append(cluster_t *cls, ushort_t col, results_t *res);

void
init_results (results_t *results);

void
free_game (game_t *game);

#endif // AMIGA_COLUMNS_CORE_H
