#ifndef TINYCOLS_PIECE_H
#define TINYCOLS_PIECE_H

#include <stdbool.h>
#include <stdint.h>

#include "color.h"
#include "grid.h"
#include "result.h"
#include "direction.h"

#define PIECE_SIZE 3

/**
 * piece - A jewel piece.
 *
 * @param row: The grid row where the top jewel of this piece is located.
 * @param col: The grid column where the top jewel of this piece is located.
 * @param status: The status of this piece, based on the result of the previous
 *                result of a movement.
 * @param colors: the colors of the jewels that form the piece.
 */
struct piece {
	int row;
	int col;
	enum result status;
	enum color colors[PIECE_SIZE];
};

/**
 * piece_randomize() - Randomize the colors of the jewels in a given piece.
 *
 * @param pc: The piece to modify.
 * @param color_max: The maximum color that can be used in the piece.
 */
void piece_randomize(struct piece *pc, unsigned int color_max);

/**
 * piece_persist() - Persist a piece in the grid according to its coordinates.
 *
 * @param pc: The piece to persist.
 * @param gr: The grid in which the piece will be persisted.
 * @return Was the piece persisted successfully?
 */
bool piece_persist(struct piece *pc, struct grid *gr);

/**
 * piece_rotate() - Rotate the jewels in the piece.
 *
 * @param pc: The piece to modify.
 * @param dir: The direction of the rotation.
 */
void piece_rotate(struct piece *pc, enum direction dir);

/**
 * piece_move_in_grid() - Move a given price in a grid.
 *
 * @param pc: The piece to move.
 * @param dir: The direction towards a movement is attempted.
 * @param gr: The grid to inspect.
 * @return The outcome of the movement
 */
enum result piece_move_in_grid(struct piece *pc, enum direction dir,
			       const struct grid *gr);

/**
 * position_piece_in_grid() - Position a piece at the starting coords of a grid.
 *
 * @param gr: The grid to inspect.
 * @param pc: The piece to position.
 * @return Was the piece positioned successfully?
 */
bool position_piece_in_grid(struct piece *pc, const struct grid *gr);
#endif //TINYCOLS_PIECE_H
