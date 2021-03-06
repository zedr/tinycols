#define LINUX_TARGET 1

#include "../core.h"
#include "minunit.h"

MU_TEST(test_scores) {
    /*
     * Lines * chain * level + 1 * 30
     * Level + 1 points soft drop
     */

    board_t board_0 = {
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
    };

    game_t *game = AllocMem(sizeof(*game), 0);

    init_game(game, DEFAULT_DIFFICULTY);

    mu_check(game->difficulty == DEFAULT_DIFFICULTY);
    mu_check((*game->board)[0] == 0);
    mu_check(game->current != NULL);
    mu_check(game->next != NULL);
    mu_check(game->score == 0);

    results_t results;
    unsigned long score = 0;

    find_clusters (&board_0, &results);
    mu_check(calc_points(&results, 0, 0) == 0);

    board_t board_3 = {
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            1, 0, 0, 0, 0, 0,
            1, 2, 0, 0, 0, 0,
            1, 2, 3, 3, 3, 0,
            1, 2, 2, 0, 0, 0,
    };

    find_clusters (&board_3, &results);
    score = calc_points(&results, 0, 0);
    char s[] = "                                  ";
    sprintf(s, "%ld != %ld", score, (unsigned long) 300);
    mu_assert(score == 300, s);

    score = calc_points(&results, 1, 0);
    sprintf(s, "%ld != %ld", score, (unsigned long) 600);
    mu_assert(score == 600, s);

    free_game(game);
}

MU_TEST(test_delete_clusters_2) {
    board_t board = {
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 1, 1, 0, 0,
    };

    piece_t piece;
    piece.x = 1;
    piece.y = 10;
    piece.jewels[0] = 1;
    piece.jewels[1] = 1;
    piece.jewels[2] = 1;

    place_piece(&piece, &board);

    results_t results;
    find_clusters(&board, &results);
    delete_clusters(&board, &results);

    for (int i = 0; i < BOARD_CELLS; i++) {
        mu_check(board[i] == 0);
    }
}

MU_TEST(test_apply_gravity) {
    board_t bb = {
            1, 0, 0, 0, 0, 0, // 0
            1, 1, 0, 1, 0, 0, // 6
            0, 1, 1, 1, 0, 0, // 12
            0, 1, 1, 0, 0, 0, // 18
            0, 0, 1, 0, 0, 0, // 24
            0, 0, 1, 1, 0, 0, // 30
            0, 0, 0, 1, 0, 0, // 36
            0, 0, 0, 0, 0, 0, // 42
            0, 0, 0, 0, 0, 0, // 48
            0, 0, 0, 0, 0, 0, // 54
            1, 0, 0, 0, 0, 0, // 60
            1, 1, 0, 0, 0, 0, // 66
            1, 1, 0, 1, 0, 0, // 72
    };

    board_t ab = {
            0, 0, 0, 0, 0, 0, // 0
            0, 0, 0, 0, 0, 0, // 6
            0, 0, 0, 0, 0, 0, // 12
            0, 0, 0, 0, 0, 0, // 18
            0, 0, 0, 0, 0, 0, // 24
            0, 0, 0, 0, 0, 0, // 30
            0, 0, 0, 0, 0, 0, // 36
            0, 0, 0, 0, 0, 0, // 42
            1, 1, 0, 1, 0, 0, // 48
            1, 1, 1, 1, 0, 0, // 54
            1, 1, 1, 1, 0, 0, // 60
            1, 1, 1, 1, 0, 0, // 66
            1, 1, 1, 1, 0, 0, // 72
    };

    int drops = apply_gravity(&bb, &move_column);

    mu_check(drops == 5);

    char *s = malloc(sizeof(*s) * 64);
    for (int i = 0; i < BOARD_CELLS; i++) {
        for (int j = 0; j < 2; j++) {
            sprintf(s, "idx %d: %d != %d", i, bb[i], ab[i]);
            mu_assert(bb[i] == ab[i], s);
        }
    }

	free(s);
}

MU_TEST(test_move_column) {
    board_t bb = {
            1, 0, 0, 0, 0, 0, // 0
            2, 1, 0, 0, 0, 0, // 6
            0, 1, 1, 0, 0, 0, // 12
            0, 1, 1, 0, 0, 0, // 18
            0, 0, 1, 0, 0, 0, // 24
            0, 0, 1, 0, 0, 0, // 30
            0, 0, 0, 0, 0, 0, // 36
            0, 0, 0, 0, 0, 0, // 42
            0, 0, 0, 0, 0, 0, // 48
            0, 0, 0, 0, 0, 0, // 54
            1, 0, 0, 0, 0, 0, // 60
            1, 1, 0, 1, 0, 0, // 66
            1, 1, 0, 1, 0, 0, // 72
    };

    board_t ab = {
            0, 0, 0, 0, 0, 0, // 0
            0, 0, 0, 0, 0, 0, // 6
            0, 0, 0, 0, 0, 0, // 12
            0, 0, 0, 0, 0, 0, // 18
            0, 0, 0, 0, 0, 0, // 24
            0, 0, 0, 0, 0, 0, // 30
            0, 0, 0, 0, 0, 0, // 36
            0, 0, 0, 0, 0, 0, // 42
            1, 1, 0, 0, 0, 0, // 48
            2, 1, 1, 0, 0, 0, // 54
            1, 1, 1, 0, 0, 0, // 60
            1, 1, 1, 1, 0, 0, // 66
            1, 1, 1, 1, 0, 0, // 72
    };

    move_column(&bb, 0, 2, 54);
    move_column(&bb, 7, 3, 61);
    move_column(&bb, 14, 4, 74);

    char *s = malloc(sizeof(*s) * 64);
    for (int i = 0; i < BOARD_CELLS; i++) {
        for (int j = 0; j < 2; j++) {
            sprintf(s, "idx %d: %d != %d", i, bb[i], ab[i]);
            mu_assert(bb[i] == ab[i], s);
        }
    }
    free(s);
}

MU_TEST(test_cluster_append) {
    ushort_t col = 1;
    results_t results;
    init_results(&results);

    cluster_t cluster;
    cluster.count = 3;
    cluster.indexes[0] = 1;
    cluster.indexes[1] = 2;
    cluster.indexes[2] = 3;

    append(&cluster, col, &results);

    mu_check(results.clusters[col - 1].count == 3);
    mu_check(results.targets[0] == 0);
    mu_check(results.targets[1] == 1);
    mu_check(results.targets[2] == 1);
    mu_check(results.targets[3] == 1);

    cluster.count = 3;
    cluster.indexes[0] = 3;
    cluster.indexes[1] = 4;
    cluster.indexes[2] = 5;

    append(&cluster, col, &results);

    mu_check(results.clusters[col - 1].count == 5);
    mu_check(results.targets[4] == 1);
    mu_check(results.targets[5] == 1);
    mu_check(results.targets[6] == 0);
    mu_check(results.clusters[col - 1].indexes[0] == 1);
    mu_check(results.clusters[col - 1].indexes[1] == 2);
    mu_check(results.clusters[col - 1].indexes[2] == 3);
    mu_check(results.clusters[col - 1].indexes[3] == 4);
    mu_check(results.clusters[col - 1].indexes[4] == 5);
}

MU_TEST(test_delete_clusters_1) {
    board_t board = {
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 1, 1, 0, 0,
    };
    piece_t piece;
    piece.x = 2;
    piece.y = 9;
    piece.jewels[0] = 1;
    piece.jewels[1] = 1;
    piece.jewels[2] = 1;
    place_piece(&piece, &board);

    mu_check(board[56] == 1);
    mu_check(board[62] == 1);
    mu_check(board[68] == 1);

    results_t results;
    init_results(&results);
    find_clusters(&board, &results);

    mu_check(results.targets[56] == 1);
    mu_check(results.targets[62] == 1);
    mu_check(results.targets[68] == 1);
    mu_check(results.targets[74] == 1);

    int count = delete_clusters(&board, &results);
    mu_check(count == 4);
    mu_check(board[56] == 0);
    mu_check(board[62] == 0);
    mu_check(board[68] == 0);
    mu_check(board[74] == 0);

    init_results(&results);
    find_clusters(&board, &results);

    mu_check(results.targets[56] == 0);
    mu_check(results.targets[62] == 0);
    mu_check(results.targets[68] == 0);
    mu_check(results.targets[74] == 0);

    mu_check(board[56] == 0);
}

MU_TEST(test_board_scan_1) {
    board_t board = {
            2, 0, 0, 1, 0, 0,
            0, 2, 1, 1, 0, 0,
            0, 1, 2, 1, 0, 0,
            0, 0, 0, 0, 0, 0,
            0, 0, 2, 0, 0, 0,
            0, 0, 0, 2, 0, 0,
            0, 0, 3, 0, 3, 0,
            2, 5, 3, 5, 3, 2,
            0, 2, 3, 0, 0, 0,
            0, 0, 2, 0, 0, 4,
            5, 5, 5, 2, 4, 0,
            0, 0, 0, 4, 2, 0,
            0, 0, 4, 0, 0, 2
    };

    results_t results;

    find_clusters(&board, &results);

    mu_check(results.clusters[0].count == 5);
    mu_check(results.clusters[1].count == 9);
    mu_check(results.clusters[2].count == 3);
    mu_check(results.clusters[3].count == 4);
    mu_check(results.clusters[4].count == 3);
}

MU_TEST(test_piece_shift_down) {
    piece_t piece;
    piece.jewels[0] = 1;
    piece.jewels[1] = 2;
    piece.jewels[2] = 3;

    shift_command(SHIFT_UP, &piece, NULL);

    mu_check(piece.jewels[0] == 2);
    mu_check(piece.jewels[1] == 3);
    mu_check(piece.jewels[2] == 1);

    shift_command(SHIFT_DOWN, &piece, NULL);
    shift_command(SHIFT_DOWN, &piece, NULL);

    mu_check(piece.jewels[0] == 3);
    mu_check(piece.jewels[1] == 1);
    mu_check(piece.jewels[2] == 2);
}

MU_TEST(test_piece_place) {
    board_t board;
    init_board(&board, NULL);
    piece_t piece = {
            {1, 2, 3},
            3,
            3
    };

    place_piece(&piece, &board);

    mu_check(inspect_board(&board, 3, 3) == 1);
    mu_check(inspect_board(&board, 3, 4) == 2);
    mu_check(inspect_board(&board, 3, 5) == 3);
}

MU_TEST(test_piece_pick) {
    game_t *game = AllocMem(sizeof(*game), 0);
	init_game (game, DEFAULT_DIFFICULTY);

    game->current->jewels[0] = ORANGE;
    game->current->jewels[1] = BLUE;
    game->current->jewels[2] = RED;
    game->next->jewels[0] = GREEN;
    game->next->jewels[1] = PURPLE;
    game->next->jewels[2] = YELLOW;

    pick_next(game, NULL);

    mu_check(game->current->jewels[0] == GREEN);
    mu_check(game->current->jewels[1] == PURPLE);
    mu_check(game->current->jewels[2] == YELLOW);

	free_game (game);

}

MU_TEST(test_setup_piece) {
    piece_t *piece = AllocMem(sizeof(*piece), 0);
    unsigned short j1 = ORANGE, j2 = GREEN, j3 = PURPLE;

    init_piece(piece, j1, j2, j3);
    piece->x = 0;
    piece->y = 0;
    mu_check(piece->jewels[0] == ORANGE);
    mu_check(piece->jewels[1] == GREEN);
    mu_check(piece->jewels[2] == PURPLE);

    FreeMem(piece, 0);
}

MU_TEST(test_setup_board) {
    board_t *board = AllocMem(sizeof(*board), 0);

    init_board(board, NULL);
    mu_check((*board)[0] == 0);
    mu_check((*board)[BOARD_CELLS - 1] == 0);

    FreeMem(board, 0);
}

MU_TEST(test_setup_game) {
    game_t *game = AllocMem(sizeof(*game), 0);

    init_game(game, DEFAULT_DIFFICULTY);

    mu_check(game->difficulty == DEFAULT_DIFFICULTY);
    mu_check((*game->board)[0] == 0);
    mu_check(game->current != NULL);
    mu_check(game->next != NULL);

    free_game(game);
}

MU_TEST_SUITE(test_suite) {
	MU_RUN_TEST(test_setup_game);
	MU_RUN_TEST(test_setup_board);
	MU_RUN_TEST(test_setup_piece);
	MU_RUN_TEST(test_piece_pick);
	MU_RUN_TEST(test_piece_place);
	MU_RUN_TEST(test_piece_shift_down);
	MU_RUN_TEST(test_cluster_append);
	MU_RUN_TEST(test_board_scan_1);
	MU_RUN_TEST(test_delete_clusters_1);
	MU_RUN_TEST(test_delete_clusters_2);
	MU_RUN_TEST(test_move_column);
	MU_RUN_TEST(test_apply_gravity);
    MU_RUN_TEST(test_scores);
}

int
main(void) {
    MU_RUN_SUITE(test_suite);
    MU_REPORT();
    puts("Test run finished.");
    return MU_EXIT_CODE;
}
