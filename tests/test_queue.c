#include "../lib/minunit.h"
#include "../src/queue.h"

void level_adder_handler(struct game *gm, unsigned int t)
{
	gm->level += t;
}

MU_TEST(test_queue_create_1)
{
	struct game gm;
	struct game_event_queue queue;
	game_queue_init(&queue, &gm);

	mu_check(queue.slots[0][0] == NULL);
}

MU_TEST(test_queue_push_1)
{
	struct game gm;
	struct game_event_queue queue;
	game_queue_init(&queue, &gm);
	int idx = 0;

	while (idx < QUEUE_SLOT_EVENTS_MAX) {
		bool was_pushed = game_queue_push(&queue, 1, level_adder_handler);
		mu_assert_int_eq(true, was_pushed);
		mu_check(*(queue.slots[1])[idx] == level_adder_handler);
		idx++;
	}
	bool was_pushed = game_queue_push(&queue, 1, level_adder_handler);
	mu_assert_int_eq(false, was_pushed);
}

MU_TEST(test_queue_dispatch_1)
{
	struct game gm;
	struct game_event_queue queue;

	gm.level = 0;

	game_queue_init(&queue, &gm);
	game_queue_push(&queue, 1, level_adder_handler);
	game_queue_push(&queue, 1, level_adder_handler);
	mu_assert_int_eq(2, game_queue_count(&queue, 1));
	int dispatched = game_queue_dispatch(&queue, 1);
	mu_assert_int_eq(2, dispatched);
	mu_assert_int_eq(2, gm.level);
	mu_assert_int_eq(0, game_queue_count(&queue, 1));
}

MU_TEST(test_queue_push_and_dispatch_with_offset_1)
{
	struct game gm;
	struct game_event_queue queue;

	gm.level = 0;
	gm.tick = 0;

	game_queue_init(&queue, &gm);
	game_queue_push(&queue, gm.tick + 50, level_adder_handler);
	mu_assert_int_eq(1,
			 game_queue_count(&queue,
					  gm.tick + 50 % QUEUE_SLOT_EVENTS_MAX));
	game_queue_dispatch(&queue, gm.tick + 50);
	game_queue_count(&queue, 0);

	gm.tick += 50;
	game_queue_push(&queue, gm.tick + 50, level_adder_handler);
	mu_assert_int_eq(1,
			 game_queue_count(&queue,
					  gm.tick + 50 % QUEUE_SLOT_EVENTS_MAX));
	game_queue_dispatch(&queue, gm.tick + 50);
	game_queue_count(&queue, 0);

	gm.tick += 50;
	game_queue_push(&queue, gm.tick + 50, level_adder_handler);
	mu_assert_int_eq(1,
			 game_queue_count(&queue,
					  gm.tick + 50 % QUEUE_SLOT_EVENTS_MAX));
	game_queue_dispatch(&queue, gm.tick + 50);
	game_queue_count(&queue, 0);
}

MU_TEST_SUITE(test_suite_queue)
{
	MU_RUN_TEST(test_queue_create_1);
	MU_RUN_TEST(test_queue_push_1);
	MU_RUN_TEST(test_queue_dispatch_1);
	MU_RUN_TEST(test_queue_push_and_dispatch_with_offset_1);
}

int main(void)
{
	MU_RUN_SUITE(test_suite_queue);
	MU_REPORT();
	return MU_EXIT_CODE;
}
