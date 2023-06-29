#include "../include/queue.h"
#include <stdlib.h>

static inline bool slot_number_is_valid(int num)
{
	return (num >= QUEUE_SLOTS_MAX || num < 0) ? false : true;
}

void game_queue_init(struct game_event_queue *qu, struct game *gm)
{
	qu->game = gm;
	for (int i = 0; i < QUEUE_SLOTS_MAX; i++) {
		for (int k = 0; k < QUEUE_SLOT_EVENTS_MAX; k++) {
			qu->slots[i][k] = NULL;
		}
	}
}

bool game_queue_push(struct game_event_queue *qu, int slot_number,
		     event_handler_t handler)
{
	event_handler_t *hnds = qu->slots[slot_number % QUEUE_SLOTS_MAX];
	for (int i = 0; i < QUEUE_SLOT_EVENTS_MAX; i++) {
		if (*hnds[i] == NULL) {
			hnds[i] = handler;
			return true;
		}
	}
	return false;
}

int game_queue_dispatch(struct game_event_queue *qu, int slot_number)
{
	if (!slot_number_is_valid(slot_number)) {
		return -1;
	}

	int count = 0;
	event_handler_t *hnds = qu->slots[slot_number];
	for (int i = 0; i < QUEUE_SLOT_EVENTS_MAX; i++) {
		if (*hnds[i] != NULL) {
			hnds[i](qu->game, slot_number);
			hnds[i] = NULL;
			count++;
		} else {
			break;
		}
	}
	return count;
}

int game_queue_count(struct game_event_queue *qu, int slot_number)
{
	if (!slot_number_is_valid(slot_number)) {
		return -1;
	}

	int count = 0;
	for (int i = 0; i < QUEUE_SLOT_EVENTS_MAX; i++) {
		if (*(qu->slots[slot_number])[i] != NULL) {
			count++;
		}
	}
	return count;
}
