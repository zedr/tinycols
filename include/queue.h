#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "tinycols/game.h"

#define QUEUE_SLOTS_MAX UINT8_MAX
#define QUEUE_SLOT_EVENTS_MAX 128

typedef void (*event_handler_t)(struct game *g, unsigned int t);

/**
 * game_event_queue - A game event queue
 *
 * @param game: The game to which this event queue is bound.
 * 		The game will be passed to the event handler upon their
 * 		invocation.
 * @param slots: The slots that contain the event handlers.
 */
struct game_event_queue {
	struct game *game;
	event_handler_t slots[QUEUE_SLOTS_MAX][QUEUE_SLOT_EVENTS_MAX];
};

/**
 * game_queue_init() - Initialize an event queue.
 *
 * @param qu: The given queue to initialize.
 */
void game_queue_init(struct game_event_queue *qu, struct game *gm);

/**
 * game_queue_push() - Push an event to the queue and bind it to a slot no.
 *
 * @param qu: The queue where the event will be pushed to.
 * @param slot_number: The chosen slot number that will be bound to the event.
 * @param handler: the handler function that will be bound to the event.
 * @return Was the event pushed to the queue successfully?
 *         If false, the chosen slot is full.
 */
bool game_queue_push(struct game_event_queue *qu,
		     int slot_number,
		     event_handler_t handler);

/**
 * game_queue_dispatch() - Dispatch the event handlers of a slot in a queue.
 *
 * Events are dispatched LIFO.
 *
 * @param qu: The queue where the event will be pushed to.
 * @param slot_number: The chosen slot number that will be bound to the event.
 *                     The number will modulo against the max lost number.
 * @return The number events that were dispatched.
 *         A negative number is returned if the slot number is invalid.
 */
int game_queue_dispatch(struct game_event_queue *qu, int slot_number);

/**
 * game_queue_count() - Count the events bound to a given slot.
 *
 * @param qu
 * @param slot_number
 * @return The number events that are bound to a slot.
 *         A negative number is returned if the slot number is invalid.
 */
int game_queue_count(struct game_event_queue *qu, int slot_number);

#endif //_QUEUE_H_
