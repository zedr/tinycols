#include <stdlib.h>
#include <stdint.h>

#include "../../include/tinycols/game.h"
#include "../../include/tinycols/game_utils.h"

unsigned short get_level_by_jewels(uint_least16_t num)
{
	return num / GAME_DEFAULT_JEWELS_FOR_LEVEL;
}

inline game_score_t calc_score(unsigned int jewel_count)
{
	return (jewel_count - PIECE_SIZE + 1) * GAME_SCORE_PER_PIECE;
}