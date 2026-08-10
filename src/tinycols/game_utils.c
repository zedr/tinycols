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
	switch (jewel_count) {
	case 3: return 30;
	case 4: return 70;
	case 5: return 150;
	default: return jewel_count * 30;
	}
}
