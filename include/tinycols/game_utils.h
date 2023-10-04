#ifndef TINYCOLS_GAME_UTILS_H
#define TINYCOLS_GAME_UTILS_H

#include <stdint.h>

#include "game.h"

/**
 * get_level_by_jewels() - Get the level based on the given number of jewels.
 *
 * @param num: The current number of jewels removed
 * @return The level number
 */
unsigned short get_level_by_jewels(uint_least16_t num);

/**
 * calc_score() - Compute the score based on the number of jewels removed.
 *
 * @param jewel_count: The number of jewels that were removed.
 * @return The computed score.
 */
game_score_t calc_score(unsigned int jewel_count);

#endif //TINYCOLS_GAME_UTILS_H
