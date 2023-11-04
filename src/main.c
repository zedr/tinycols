#include <ncurses.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "../include/tinycols.h"
#include "../include/queue.h"
#include "../include/gfx.h"

#define TICK_TIME 10000
#define MAX_TIMER 180

WINDOW *win;
uint8_t tmp_res[GRID_DEFAULT_COLS * GRID_DEFAULT_ROWS];
struct grid_drop tmp_drs[GRID_DEFAULT_COLS * GRID_DEFAULT_ROWS];
struct game_event_queue qu;
int tick_time;
int chain_num;
bool debug_mode = false;

static void scan_grid(struct game *gm, unsigned int t);

/**
 * process_keys() - Process any keypress generated by the game player.
 *
 * @param gm: The game.
 */
void process_keys(struct game *gm)
{
	int ch = wgetch(win);
	if (ch == 'p') {
		gm->status = (gm->status == GAME_PAUSED) ? GAME_READY :
								 GAME_PAUSED;
	} else if (ch == 'q') {
		gm->status = GAME_EXIT;
	}

	if (gm->status > GAME_READY) {
		return;
	}

	switch (ch) {
	case KEY_RIGHT:
		piece_move_in_grid(&gm->current_piece, RIGHT, gm->grid);
		break;
	case KEY_LEFT:
		piece_move_in_grid(&gm->current_piece, LEFT, gm->grid);
		break;
	case KEY_DOWN:
		if ((gm->current_piece.status != PENDING) &&
		    (piece_move_in_grid(&gm->current_piece, DOWN, gm->grid) ==
		     MOVED)) {
			gm->score++;
		}
		break;
	case 'a':
	case KEY_UP:
		piece_rotate(&gm->current_piece, UP);
		break;
	case 'z':
		piece_rotate(&gm->current_piece, DOWN);
		break;
	case 'q':
		break;
	default:
		break;
	}

	flushinp();
}

/**
 * get_tick_time() - Get the tick time based on the level number.
 *
 * @param level: The level number
 * @return The tick time number
 */
static uint8_t get_tick_time(unsigned short level)
{
	return (uint8_t)MAX_TIMER / (level + 1);
}

/**
 * move_piece_down() - Move the piece down (game gravity)
 *
 * @param gm: The game to update
 * @param t: The current tick
 */
static void move_piece_down(struct game *gm, unsigned int t)
{
	if (gm->current_piece.status < LANDED) {
		piece_move_in_grid(&gm->current_piece, DOWN, gm->grid);
	}
	game_queue_push(&qu, t + tick_time, move_piece_down);
}

/**
 *
 *
 * @param gm
 * @param t
 */
static void remove_jewels(struct game *gm, unsigned int t)
{
	gm->jewels_removed += grid_remove_jewels(gm->grid, tmp_res);
	gm->level = get_level_by_jewels(gm->jewels_removed);
	tick_time = get_tick_time(gm->level);
	unsigned int count;
	while ((count = grid_detect_drops(gm->grid, tmp_drs, gm->grid->size))) {
		grid_apply_drops(gm->grid, tmp_drs, count);
	}
	gm->current_piece.status = INVISIBLE;
	game_queue_push(&qu, t + 25, scan_grid);
}

static void activate(struct game *gm, unsigned int t)
{
	gm->current_piece.status = ACTIVE;
}

static void scan_grid(struct game *gm, unsigned int t)
{
	gm->current_piece.status = PERSISTED;
	if ((gm->last_score = grid_scan(gm->grid, tmp_res)) > 0) {
		chain_num++;
		gm->last_score *= (gm->level + 1) * chain_num;
		gm->score += gm->last_score;
		game_queue_push(&qu, t + 50, remove_jewels);
	} else {
		chain_num = 0;
		game_cycle_piece(gm);
		position_piece_in_grid(&gm->current_piece, gm->grid);
		gm->current_piece.status = PENDING;
		game_queue_push(&qu, t + 50, activate);
	}
}

/**
 * game_tick() - Process a single tick of a game.
 *
 * @param gm: The game.
 */
void game_tick(struct game *gm)
{
	if (gm->current_piece.status == UNKNOWN) {
		game_queue_push(&qu, gm->tick + get_tick_time(gm->level),
				move_piece_down);
		gm->current_piece.status = ACTIVE;
	}

	if (gm->current_piece.status == LANDED) {
		if (piece_persist(&gm->current_piece, gm->grid)) {
			chain_num = 0;
			game_queue_push(&qu, gm->tick, scan_grid);

		} else {
			gm->status = GAME_OVER;
		}
	}

	game_queue_dispatch(&qu, gm->tick);
	gm->tick++;
}

/**
 * get_delay_usec() - Get the delay for a tick in microseconds.
 *
 * If the result of the computation is negative, return zero.
 *
 * @param start
 * @param end
 * @return
 */
static uint32_t get_delay_usec(struct timeval start, struct timeval end)
{
	time_t sec = end.tv_sec - start.tv_sec;
	int32_t t = start.tv_usec + TICK_TIME - (sec * 1000000) - end.tv_usec;
	return (t >= 0) ? (uint32_t)t : 0;
}

/**
 * run() - Run the game.
 */
static game_score_t run(enum game_class cls)
{
	win = setup_gfx();

	struct game *gm = game_alloc();
	if (gm == NULL) {
		perror("Out of memory");
		exit(EXIT_FAILURE);
	}
	game_init(gm, GAME_DEFAULT_LEVEL, cls);
	game_queue_init(&qu, gm);

	struct timeval time_start, time_end;
	tick_time = get_tick_time(gm->level);

	while (gm->status < GAME_OVER) {
		// Time Start
		gettimeofday(&time_start, NULL);

		// Process input
		if (gm->current_piece.status < LANDED) {
			process_keys(gm);
		}

		// Process game logic
		if (gm->status != GAME_PAUSED) {
			game_tick(gm);
		}

		// Render game
		draw_frame(gm, 0, 0);
		draw_game(gm, 0, 0);

		if (gm->current_piece.status == PERSISTED) {
			draw_stars(tmp_res, gm->grid, 0, 0);
		} else if (gm->current_piece.status != INVISIBLE) {
			draw_piece(&gm->current_piece, 1, 1);
		}
		draw_debug(gm, gm->grid->cols * 2 + 3, 5);
		refresh();

		// Time End
		gettimeofday(&time_end, NULL);
		usleep(get_delay_usec(time_start, time_end));
	}

	if (gm->status == GAME_OVER) {
		usleep(1000000);
	}
	game_score_t final_score = gm->score;
	game_free(gm);
	teardown_gfx(win);
	return final_score;
}

/**
 * init_rand() - Initialize the random number generator
 */
static void init_rand(void)
{
	struct timeval time_now;
	gettimeofday(&time_now, NULL);
	srand((unsigned int)time_now.tv_usec); // NOLINT(cert-msc51-cpp)
}

static void usage(char *prog_name)
{
	fprintf(stderr, "Usage:\n  %s [-c class]\n", prog_name);
	fputs("\n", stderr);
	fputs("Options:\n", stderr);
	fputs("  -h\t\tPrint this help message.\n", stderr);
	fputs("  -c[N]\t\tSelect game class number N: "
	      "1 (Novice), "
	      "2 (Amateur), "
	      "3 (Pro). "
	      "The default is 2.\n",
	      stderr);
}

int main(int argc, char **argv)
{
	init_rand();
	int opt;
	enum game_class cls = CLASS_AMATEUR;

	char *prog_name = argv[0];
	while ((opt = getopt(argc, argv, "c:dh")) != -1) {
		switch (opt) {
		case 'h': {
			usage(prog_name);
			exit(EXIT_SUCCESS);
		}
		case 'c': {
			const int num = atoi(optarg);
			if (num >= 1 && num <= 3) {
				cls = (enum game_class)(num + 3);
			} else {
				fprintf(stderr, "Error: class must be between "
						"1 and 3.\n");
				exit(EXIT_FAILURE);
			}
			break;
		}
		case 'd': {
			debug_mode = true;
			break;
		}
		default:
			usage(prog_name);
			exit(EXIT_FAILURE);
		}
	}

	printf("Final score: %hu\n", run(cls));
	return 0;
}
