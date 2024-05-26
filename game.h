/*
 * game.h
 *
 *  Important game structures, constants, and definitions.
 */

#ifndef FOURKNIGHTS_GAME_H
#define FOURKNIGHTS_GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "queue.h"
#include "list.h"


#define BOARD_SIZE          9
#define MAX_POSSIBLE_MOVES  2

#define MIN(x,y) \
    ((x) < (y) ? (x) : (y))
#define MAX(x,y) \
    ((x) > (y) ? (x) : (y))

#ifdef FN_DEBUG
#define debug(x,...) \
    printf(x, ##__VA_ARGS__);
#else
#define debug(x,...)
#endif

#define PRINT(x,...) \
    printf(x, ##__VA_ARGS__);

/* This game uses a tri-state system for each space. */
typedef enum
{
    EMPTY = 0,
    BLACK_1,
    BLACK_2,
    WHITE_1,
    WHITE_2
} board_space_state_t;

/*
 * Board layout (0-indexed, so a1 = 0, a2 = 1, etc.):
 * +----+----+----+
 * | a1 | a2 | a3 |
 * +----+----+----+
 * | b1 | b2 | b3 |
 * +----+----+----+
 * | c1 | c2 | c3 |
 * +----+----+----+
 */
typedef struct _board board_t;
struct _board {
    union {
        struct {
            board_space_state_t a1;
            board_space_state_t a2;
            board_space_state_t a3;
            board_space_state_t b1;
            board_space_state_t b2;
            board_space_state_t b3;
            board_space_state_t c1;
            board_space_state_t c2;
            board_space_state_t c3;
        };
        board_space_state_t s[BOARD_SIZE];
    };
    board_t *parent_state;
    unsigned int moves_from_start;
};


/* Meta-details about the current game. */
typedef struct _game
{
    board_t            *current_board_state;
    board_t             initial_board_state;
    board_t             goal_board_state;
    list_t             *solution_path;
    list_t             *visited_boards;
    queue_t            *priority_queue;
//    queue_t            *visited_queue;
    unsigned int        expansions;
} game_t;


/* Create the set of legal game moves. */
typedef struct { int destinations[2]; } move_t;
static move_t move[BOARD_SIZE] = {
        { .destinations = {5, 7} },
        { .destinations = {6, 8} },
        { .destinations = {3, 7} },
        { .destinations = {2, 8} },
        { .destinations = {-1, -1} }, /* #4 (b2) is unused. */
        { .destinations = {0, 6} },
        { .destinations = {1, 5} },
        { .destinations = {0, 2} },
        { .destinations = {1, 3} },
};


/* File-wide global game object. */
static
game_t *
        four_knights;


/* Initialize a board state to its default for the puzzle. */
static inline
void
reset_game(game_t *game)
{
    free(game->current_board_state);
    game->current_board_state = (board_t *)malloc(sizeof(board_t));

    memcpy(game->current_board_state,
           &game->initial_board_state,
           sizeof(board_t));

    game->expansions = 0;

    queue__destroy(&game->priority_queue);
    list__destroy(&game->solution_path, 0);
    list__destroy(&game->visited_boards, 1);

    game->priority_queue = queue__create(1 << 22);
    game->solution_path = list__create();
    game->visited_boards = list__create();
}


/* Compare the board state to its goal state. */
static inline
int
check_game(game_t *game)
{
    return memcmp(game->current_board_state->s,
                  game->goal_board_state.s,
                  BOARD_SIZE * sizeof(board_space_state_t));
}


/* Print the current board state. */
static
void
print_board(board_t *board)
{
    /* '9' is used here because it is the total size of all board states. */
    for (int i = 0; i < BOARD_SIZE; ++i) {
        switch (board->s[i]) {
            case EMPTY: debug("."); break;
            case BLACK_1: debug("B"); break;
            case BLACK_2: debug("b"); break;
            case WHITE_1: debug("W"); break;
            case WHITE_2: debug("w"); break;
        }
        /* Print a line break if the indexer is a multiple of 3 and above 0. */
        if (i > 0 && !((i+1) % 3)) debug("\n");
    }
}


/* Print the series of moves discovered by the search. */
static
void
print_final_game_solution(game_t *game)
{
    debug("\n\n\n========================================\nFinal game route (%u steps):\n",
           game->current_board_state->moves_from_start);

    /* Create a linked list from the series of board states going backward. */
    board_t *prev_board = game->current_board_state;
    while (NULL != prev_board) {
        list__insert(game->solution_path, prev_board);
        prev_board = prev_board->parent_state;
    }

    /* Reverse the list so it becomes a forward path. */
    list__reverse(game->solution_path);

    /* Traverse the list and print each board state. */
    list_node_t *node = game->solution_path->head;
    while (NULL != node) {
        print_board((board_t *)node->node);
        debug("\n");
        node = node->next;
    }
}


/* Pseudo-hashing function identifying unique board states. */
static
unsigned int
get_state_code(board_t *board)
{
    unsigned int state_code = 0;

    /* The '5.0' here comes from the fact that each space can be in
     * one of 5 different states based on EMPTY or the piece. */
    for (int k = 0; k < BOARD_SIZE; k++)
        state_code += (pow(5.0, (1.0 * k)) * board->s[k]);

    return state_code;
}


/* Get the estimated value of h(x) for a legal board state. */
static
unsigned int
get_heuristic(board_t *next_state,
              game_t  *game)
{
    /*
     * ALL Four Knights puzzles create a node graph with a cyclical
     * set of legal state transitions. This is a cyclic number line.
     * At any given time, the heuristic H(x) measurement is defined
     * by how far away on this CYCLE GRAPH the current point is from
     * its desired destination in the cycle.
     *
     * See: https://mindyourdecisions.com/blog/wp-content/uploads/2014/03/four-knights-puzzle-solution-final-graph.png
     */
    int cycle[8] = { 6, 1, 8, 3, 2, 7, 0, 5 };
    unsigned int h_x = 0;

    for (int i = 0; i < BOARD_SIZE; ++i) {
        /* Skip empty goal places, where there is no measurement to make. */
        if (EMPTY == game->goal_board_state.s[i]) continue;

        for (int j = 0; j < BOARD_SIZE; ++j) {
            /* If the type of piece doesn't match here for 'j', continue till it does. */
            if (next_state->s[j] != game->goal_board_state.s[i]) continue;

            /* Find the index in the cycle for each value. */
            int i_curr = 0, i_goal = 0;
            while (cycle[i_goal] != i) ++i_goal;
            while (cycle[i_curr] != j) ++i_curr;

            /* Add the absolute distance in the cycle from the current location
             * to the desired location in the goal state. */
            h_x += MIN(abs(i_goal - i_curr), 8 - abs(i_goal - i_curr));
        }
    }

    return h_x;
}


#endif   /* FOURKNIGHTS_GAME_H */