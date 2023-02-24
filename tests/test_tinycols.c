#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#include "../lib/minunit.h"

#include "../lib/helpers.h"
#include "../src/tinycols.h"

MU_TEST(test_grid_create_and_init_1)
{
	struct grid *gr = grid_alloc(3, 2);
	if (gr == NULL) {
		mu_fail("Out of memory");
	}
	grid_init(gr);

	mu_assert_int_eq(3, gr->rows);
	mu_assert_int_eq(2, gr->cols);
	mu_assert_int_eq(3 * 2, gr->size);
	mu_assert_int_eq(TRANSPARENT, test_get_cell(gr, 0, 0));

	free(gr);
}

MU_TEST(test_grid_get_cell_1)
{
	uint8_t _cells[] = {1, 2, 3, 4, 5, 6};
	struct grid *gr = grid_alloc(2, 3);
	memcpy(gr->cells, &_cells, gr->size);

	mu_assert_int_eq(1, test_get_cell(gr, 0, 0));
	mu_assert_int_eq(2, test_get_cell(gr, 0, 1));
	mu_assert_int_eq(3, test_get_cell(gr, 0, 2));
	mu_assert_int_eq(4, test_get_cell(gr, 1, 0));
	mu_assert_int_eq(5, test_get_cell(gr, 1, 1));
	mu_assert_int_eq(6, test_get_cell(gr, 1, 2));

	free(gr);
}

MU_TEST(test_grid_set_cell_1)
{
	uint8_t _cells[] = {0, 0, 0, 0, 0, 0};
	struct grid *gr = grid_alloc(2, 3);
	memcpy(&_cells, gr->cells, gr->size);

	test_set_cell(gr, 0, 0, ORANGE);
	test_set_cell(gr, 1, 0, BLUE);

	mu_assert_int_eq(ORANGE, test_get_cell(gr, 0, 0));
	mu_assert_int_eq(BLUE, test_get_cell(gr, 1, 0));

	free(gr);
}

MU_TEST(test_grid_scan_1)
{
	uint8_t _cells[] = {ORANGE, TRANSPARENT, TRANSPARENT,
			    ORANGE, GREEN,	 TRANSPARENT,
			    ORANGE, ORANGE,	 ORANGE};
	struct grid *gr = grid_alloc(3, 3);
	memcpy(gr->cells, &_cells, gr->size);
	uint8_t expected_found[] = {ORANGE, TRANSPARENT, TRANSPARENT,
				    ORANGE, TRANSPARENT, TRANSPARENT,
				    ORANGE, ORANGE,	 ORANGE};
	uint8_t found[9] = {TRANSPARENT};
	score_t score = grid_scan(gr, (uint8_t *)&found);
	mu_assert_int_eq(60, (unsigned long)score);
	mu_check(compare((uint8_t *)&expected_found, (uint8_t *)&found, 9));

	free(gr);
}

MU_TEST(test_grid_scan_2)
{
	uint8_t _cells[] = {ORANGE,	 GREEN, ORANGE, ORANGE, ORANGE,
			    TRANSPARENT, GREEN, GREEN,	ORANGE};
	struct grid *gr = grid_alloc(3, 3);
	memcpy(gr->cells, &_cells, gr->size);
	uint8_t expected_found[] = {ORANGE,	 TRANSPARENT, TRANSPARENT,
				    TRANSPARENT, ORANGE,      TRANSPARENT,
				    TRANSPARENT, TRANSPARENT, ORANGE};
	uint8_t found[9] = {TRANSPARENT};
	unsigned long score = grid_scan(gr, (uint8_t *)&found);
	mu_assert_int_eq(30, score);
	mu_check(compare((uint8_t *)&expected_found, (uint8_t *)&found, 9));

	free(gr);
}

MU_TEST(test_grid_scan_3)
{
	uint8_t _cells[] = {GREEN,	 ORANGE, ORANGE,
			    TRANSPARENT, ORANGE, TRANSPARENT,
			    TRANSPARENT, ORANGE, TRANSPARENT};
	struct grid *gr = grid_alloc(3, 3);
	memcpy(gr->cells, &_cells, gr->size);
	uint8_t expected_found[] = {TRANSPARENT, ORANGE, TRANSPARENT,
				    TRANSPARENT, ORANGE, TRANSPARENT,
				    TRANSPARENT, ORANGE, TRANSPARENT};
	uint8_t found[9] = {TRANSPARENT};
	unsigned long score = grid_scan(gr, (uint8_t *)&found);
	mu_assert_int_eq(30, score);
	mu_check(compare((uint8_t *)&expected_found, (uint8_t *)&found, 9));

	free(gr);
}

MU_TEST(test_grid_scan_4)
{
	uint8_t _cells[] = {TRANSPARENT, TRANSPARENT, ORANGE,
			    TRANSPARENT, ORANGE,      TRANSPARENT,
			    ORANGE,	 GREEN,	      GREEN};
	struct grid *gr = grid_alloc(3, 3);
	memcpy(gr->cells, &_cells, gr->size);
	uint8_t expected_found[] = {TRANSPARENT, TRANSPARENT, ORANGE,
				    TRANSPARENT, ORANGE,      TRANSPARENT,
				    ORANGE,	 TRANSPARENT, TRANSPARENT};
	uint8_t found[9] = {TRANSPARENT};
	unsigned long score = grid_scan(gr, (uint8_t *)&found);
	mu_assert_int_eq(30, score);
	mu_check(compare((uint8_t *)&expected_found, (uint8_t *)&found, 9));

	free(gr);
}

MU_TEST(test_grid_remove_jewels_1)
{
	uint8_t _cells[] = {TRANSPARENT, ORANGE, ORANGE, TRANSPARENT, ORANGE,
			    GREEN,	 ORANGE, ORANGE, GREEN};
	struct grid *gr = grid_alloc(3, 3);
	memcpy(gr->cells, &_cells, gr->size);
	uint8_t expected[] = {TRANSPARENT, TRANSPARENT, TRANSPARENT,
			      TRANSPARENT, TRANSPARENT, GREEN,
			      TRANSPARENT, TRANSPARENT, GREEN};
	uint8_t found[9] = {TRANSPARENT};
	(void)grid_scan(gr, (uint8_t *)&found);
	grid_remove_jewels(gr, found);
	mu_check(compare((uint8_t *)&expected, gr->cells, 9));

	free(gr);
}

MU_TEST(test_grid_detect_drops_1)
{
	uint8_t _cells[] = {GREEN,	 ORANGE,      ORANGE,
			    TRANSPARENT, ORANGE,      GREEN,
			    TRANSPARENT, TRANSPARENT, GREEN};
	struct grid *gr = grid_alloc(3, 3);
	memcpy(gr->cells, &_cells, gr->size);
	struct drop drs[2] = {0};
	unsigned int count = grid_detect_drops(gr, drs, 2);
	mu_assert_int_eq(2, count);
	mu_assert_int_eq(1, drs[0].col);
	mu_assert_int_eq(0, drs[0].row);
	mu_assert_int_eq(2, drs[0].n);
	mu_assert_int_eq(1, drs[0].h);
	mu_assert_int_eq(0, drs[1].col);
	mu_assert_int_eq(0, drs[1].row);
	mu_assert_int_eq(1, drs[1].n);
	mu_assert_int_eq(2, drs[1].h);

	free(gr);
}

MU_TEST(test_grid_apply_drops_1)
{
	uint8_t _cells[] = {GREEN,	 ORANGE,      ORANGE,
			    TRANSPARENT, ORANGE,      GREEN,
			    TRANSPARENT, TRANSPARENT, GREEN};
	struct grid *gr = grid_alloc(3, 3);
	memcpy(gr->cells, &_cells, gr->size);
	struct drop drs[2] = {0};
	uint8_t expected[] = {TRANSPARENT, TRANSPARENT, ORANGE,
			      TRANSPARENT, ORANGE,	GREEN,
			      GREEN,	   ORANGE,	GREEN};
	unsigned int count = grid_detect_drops(gr, drs, 2);
	mu_assert_int_eq(2, count);
	grid_apply_drops(gr, drs, 2);
	mu_check(compare((uint8_t *)&expected, gr->cells, 9));

	free(gr);
}

MU_TEST(test_grid_position_piece_1)
{
	struct grid *gr = grid_alloc(6, 3);
	grid_init(gr);
	struct piece pc = {
		.row = 0, .col = 0, .colors = {GREEN, YELLOW, BLUE}};

	mu_assert_int_eq(true, grid_position_piece(gr, &pc));
	mu_assert_int_eq(1, pc.col);
	mu_assert_int_eq(-2, pc.row);

	struct piece pc2 = {
		.row = 0, .col = 1, .colors = {PURPLE, RED, ORANGE}};
	piece_persist(&pc2, gr);

	mu_assert_int_eq(false, grid_position_piece(gr, &pc));

	free(gr);
}

MU_TEST(test_piece_randomize_1)
{
	struct piece pc = {.row = 0,
			   .col = 0,
			   .colors = {TRANSPARENT, TRANSPARENT, TRANSPARENT}};

	piece_randomize(&pc);

	mu_assert(pc.colors[0] != TRANSPARENT, "Failed index 0: is not none");
	mu_assert(pc.colors[0] <= COLOR_MAX, "Failed index 0: unknown color");
	mu_assert(pc.colors[1] != TRANSPARENT, "Failed index 1: is not none");
	mu_assert(pc.colors[1] <= COLOR_MAX, "Failed index 1: unknown color");
	mu_assert(pc.colors[2] != TRANSPARENT, "Failed index 2: is not none");
	mu_assert(pc.colors[2] <= COLOR_MAX, "Failed index 2: unknown color");
}

MU_TEST(test_piece_position_ok_1)
{
	struct grid *gr = grid_alloc(6, 6);
	grid_init(gr);

	struct piece pc = {.row = 0,
			   .col = 0,
			   .status = UNKNOWN,
			   .colors = {ORANGE, BLUE, YELLOW}};

	mu_check(piece_persist(&pc, gr));
	mu_assert_int_eq(PERSISTED, pc.status);
	mu_assert_int_eq(ORANGE, test_get_cell(gr, 0, 0));
	mu_assert_int_eq(BLUE, test_get_cell(gr, 1, 0));
	mu_assert_int_eq(YELLOW, test_get_cell(gr, 2, 0));

	free(gr);
}

MU_TEST(test_piece_position_fail_1)
{
	struct grid *gr = grid_alloc(6, 6);

	struct piece pc = {
		.row = -1, .col = 0, .colors = {ORANGE, BLUE, YELLOW}};

	mu_check(piece_persist(&pc, gr) == false);
	pc.row = 0;
	pc.col = -1;
	mu_check(piece_persist(&pc, gr) == false);
	pc.row = 0;
	pc.col = gr->cols;
	mu_check(piece_persist(&pc, gr) == false);
	pc.row = gr->rows;
	pc.col = 0;
	mu_check(piece_persist(&pc, gr) == false);

	free(gr);
}

MU_TEST(test_piece_move_ok_1)
{
	struct grid *gr = grid_alloc(6, 6);
	grid_init(gr);

	struct piece pc = {
		.row = 0, .col = 0, .colors = {ORANGE, BLUE, YELLOW}};

	mu_assert_int_eq(MOVED, piece_move_in_grid(&pc, DOWN, gr));
	mu_assert_int_eq(MOVED, pc.status);
	mu_assert_int_eq(1, pc.row);
	pc.row = gr->rows - PIECE_SIZE;
	mu_assert_int_eq(LANDED, piece_move_in_grid(&pc, DOWN, gr));
	mu_assert_int_eq(LANDED, pc.status);
	mu_assert_int_eq(MOVED, piece_move_in_grid(&pc, RIGHT, gr));
	mu_assert_int_eq(MOVED, pc.status);
	mu_assert_int_eq(1, pc.col);
	mu_assert_int_eq(MOVED, piece_move_in_grid(&pc, LEFT, gr));
	mu_assert_int_eq(MOVED, pc.status);
	mu_assert_int_eq(0, pc.col);

	free(gr);
}

MU_TEST(test_piece_move_ok_2)
{
	struct grid *gr = grid_alloc(13, 6);
	grid_init(gr);

	struct piece pc = {
		.row = 0, .col = 0, .colors = {ORANGE, BLUE, YELLOW}};
	grid_position_piece(gr, &pc);
	mu_assert_int_eq(-2, pc.row);
	mu_assert_int_eq(MOVED, piece_move_in_grid(&pc, DOWN, gr));
	mu_assert_int_eq(-1, pc.row);

	free(gr);
}

MU_TEST(test_piece_move_fail_1)
{
	uint8_t _cells[] = {
		TRANSPARENT, TRANSPARENT, TRANSPARENT, TRANSPARENT,
		TRANSPARENT, TRANSPARENT, TRANSPARENT, TRANSPARENT,
		TRANSPARENT, TRANSPARENT, ORANGE,      TRANSPARENT,
		TRANSPARENT, RED,	  TRANSPARENT, TRANSPARENT,
		BLUE,	     TRANSPARENT,
	};
	struct grid *gr = grid_alloc(6, 3);
	memcpy(gr->cells, &_cells, gr->size);
	struct piece pc = {
		.row = 3, .col = 0, .colors = {PURPLE, YELLOW, GREEN}};

	mu_check(piece_move_in_grid(&pc, LEFT, gr) == BLOCKED);
	mu_check(piece_move_in_grid(&pc, RIGHT, gr) == BLOCKED);
	pc.row--;
	mu_check(piece_move_in_grid(&pc, RIGHT, gr) == BLOCKED);
	pc.row--;
	mu_check(piece_move_in_grid(&pc, RIGHT, gr) == BLOCKED);
	pc.row--;
	mu_check(piece_move_in_grid(&pc, RIGHT, gr) == MOVED);
	mu_check(piece_move_in_grid(&pc, DOWN, gr) == LANDED);
	pc.row = 3;
	pc.col = 2;
	mu_check(piece_move_in_grid(&pc, LEFT, gr) == BLOCKED);
	pc.row--;
	mu_check(piece_move_in_grid(&pc, LEFT, gr) == BLOCKED);
	pc.row--;
	mu_check(piece_move_in_grid(&pc, LEFT, gr) == BLOCKED);
	pc.row--;
	mu_check(piece_move_in_grid(&pc, LEFT, gr) == MOVED);

	free(gr);
}

MU_TEST(test_piece_rotate_1)
{
	struct piece pc = {
		.row = 0, .col = 0, .colors = {PURPLE, YELLOW, GREEN}};

	piece_rotate(&pc, DOWN);
	mu_assert_int_eq(GREEN, pc.colors[0]);
	mu_assert_int_eq(PURPLE, pc.colors[1]);
	mu_assert_int_eq(YELLOW, pc.colors[2]);
	piece_rotate(&pc, UP);
	mu_assert_int_eq(PURPLE, pc.colors[0]);
	mu_assert_int_eq(YELLOW, pc.colors[1]);
	mu_assert_int_eq(GREEN, pc.colors[2]);
}

MU_TEST(test_game_init_1)
{
	struct game *gm = game_alloc();
	game_init(gm, GAME_DEFAULT_LEVEL, GAME_DEFAULT_COLOR_MAX);

	mu_assert_int_eq(GAME_DEFAULT_LEVEL, gm->level);
	mu_assert_int_eq(0, gm->score);
	mu_assert_int_eq(GAME_DEFAULT_COLOR_MAX, gm->color_max);
	mu_assert_int_eq(GAME_READY, gm->status);
	mu_assert_int_eq(UNKNOWN, gm->current_piece.status);
	mu_check(gm->current_piece.colors[0] > 0);
	mu_check(gm->current_piece.colors[0] <= COLOR_MAX);
	mu_check(gm->current_piece.colors[1] > 0);
	mu_check(gm->current_piece.colors[1] <= COLOR_MAX);
	mu_check(gm->current_piece.colors[2] > 0);
	mu_check(gm->current_piece.colors[2] <= COLOR_MAX);
	mu_check(gm->next_piece.colors[0] > 0);
	mu_check(gm->next_piece.colors[0] <= COLOR_MAX);
	mu_check(gm->next_piece.colors[1] > 0);
	mu_check(gm->next_piece.colors[1] <= COLOR_MAX);
	mu_check(gm->next_piece.colors[2] > 0);
	mu_check(gm->next_piece.colors[2] <= COLOR_MAX);

	game_free(gm);
}

MU_TEST(test_game_set_level_1)
{
	mu_assert_int_eq(0, get_level_by_jewels(0));
	mu_assert_int_eq(0, get_level_by_jewels(5));
	mu_assert_int_eq(1, get_level_by_jewels(30));
	mu_assert_int_eq(1, get_level_by_jewels(31));
	mu_assert_int_eq(2, get_level_by_jewels(60));

	struct game *gm = game_alloc();
	game_init(gm, GAME_DEFAULT_LEVEL, GAME_DEFAULT_COLOR_MAX);

	gm->level = get_level_by_jewels(31);
	mu_assert_int_eq(1, gm->level);

	game_free(gm);
}

MU_TEST_SUITE(test_suite_grid)
{
	MU_RUN_TEST(test_grid_create_and_init_1);
	MU_RUN_TEST(test_grid_get_cell_1);
	MU_RUN_TEST(test_grid_set_cell_1);
	MU_RUN_TEST(test_grid_scan_1);
	MU_RUN_TEST(test_grid_scan_2);
	MU_RUN_TEST(test_grid_scan_3);
	MU_RUN_TEST(test_grid_scan_4);
	MU_RUN_TEST(test_grid_remove_jewels_1);
	MU_RUN_TEST(test_grid_detect_drops_1);
	MU_RUN_TEST(test_grid_apply_drops_1);
	MU_RUN_TEST(test_grid_position_piece_1);
}

MU_TEST_SUITE(test_suite_piece)
{
	MU_RUN_TEST(test_piece_randomize_1);
	MU_RUN_TEST(test_piece_position_ok_1);
	MU_RUN_TEST(test_piece_position_fail_1);
	MU_RUN_TEST(test_piece_move_ok_1);
	MU_RUN_TEST(test_piece_move_ok_2);
	MU_RUN_TEST(test_piece_move_fail_1);
	MU_RUN_TEST(test_piece_rotate_1);
}
MU_TEST_SUITE(test_suite_game)
{
	MU_RUN_TEST(test_game_init_1);
	MU_RUN_TEST(test_game_set_level_1);
}

int main(int argc, char *argv[])
{
	MU_RUN_SUITE(test_suite_grid);
	MU_RUN_SUITE(test_suite_piece);
	MU_RUN_SUITE(test_suite_game);
	MU_REPORT();
	return MU_EXIT_CODE;
}
