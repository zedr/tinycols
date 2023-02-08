#include "helpers.h"

bool compare(uint8_t *first, uint8_t *second, int len)
{
	for (size_t i = 0; i < len; i++) {
		if (first[i] != second[i]) {
			return false;
		}
	}
	return true;
}

