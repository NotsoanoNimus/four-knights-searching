/*
 * The Four Knights Puzzle
 *      Comparing A* and Branch-and-Bound searches in solving the puzzle.
 *
 *  Author: Zack Puhl <zpuhl@umich.edu>
 *  Date  : May 25, 2024
 *
 *  Runs a simulation of the Four Knights puzzle with A* and
 *  branch-and-bound searching, then compares the results.
 */

#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


/* A*: Calculate a list of all possible next states from the current one. */
static
void
astar__get_next_possible_moves(game_t *game)
{
    /*
     * Quick notes about the rules for Four Knights:
     * - The middle space (b2 / #4) is never used as a reachable location.
     * - Possible state transitions follow a map, but need to only
     *   be allowed if the destination is EMPTY.
     * - Knights can usually only ever move to two spaces from current.
     */
    board_t *current_state = game->current_board_state;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        /* No moves generated from an empty slot. */
        if (EMPTY == current_state->s[i]) continue;
        /* No moves from position #4 (b2). */
        if (4 == i) continue;

        /* For each occupied slot, check the allowable destinations
         * in the graph. If EMPTY, create a new board state from the
         * moved knight. */
        for (int j = 0; j < MAX_POSSIBLE_MOVES; ++j) {
            /* If the destination slot is occupied, the move cannot be completed. */
            if (EMPTY != current_state->s[move[i].destinations[j]]) continue;

            /* Create a new board state from the expansion. */
            board_t *new_state = calloc(1, sizeof(board_t));
            memcpy(new_state, current_state, sizeof(board_t));

            /* Move the piece by getting what the old space held
             * and applying it to the new space. The old space becomes EMPTY. */
            board_space_state_t old_state = new_state->s[i];
            new_state->s[i] = EMPTY;
            new_state->s[move[i].destinations[j]] = old_state;

            /* Track the parent state we expanded from. */
            new_state->parent_state = current_state;
            new_state->moves_from_start = current_state->moves_from_start + 1;

            /* Make sure this new possible state has not already been visited. */
            unsigned int state_code = get_state_code(new_state);
            if (0 == list__contains(game->visited_boards,
                                    &state_code,
                                    sizeof(unsigned int)))
            {
                debug("\nDiscovered new possible move:\n");
                print_board(new_state);

                /*
                 * F(x) is the total cost estimate.
                 * G(x) is the amount of moves away from the origin for this expansion.
                 * H(x) is the heuristic (or 'closeness') measurement.
                 */
                unsigned int h_x = get_heuristic(new_state, game);
                unsigned int g_x = new_state->moves_from_start;
                unsigned int f_x = g_x + h_x;

                debug("Stats:\tf(x) = %u\tg(x) = %u\th(x) = %u\n", f_x, g_x, h_x);
                debug("\tUnseen board hash recorded: %u\n", state_code);

                /* Be sure to insert the board state into the priority-based queue structure. */
                queue__insert(game->priority_queue,
                              new_state,
                              f_x,
                              g_x,
                              h_x);

                /*
                 * Notice that A* doesn't track every board state as visited;
                 * only the ones it chooses from the min_queue.
                 * Instead, it can rely on the H(x) value to guide it to the
                 * end state that represents a completed game.
                 * */
            }
        }
    }
}


/* Branch & Bound: Calculate a list of all possible next states from the current one. */
static
void
bnb__get_next_possible_moves(game_t *game)
{
    /* Same rules apply as in the A* function. See that function for most annotations. */
    board_t *current_state = game->current_board_state;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        if (EMPTY == current_state->s[i]) continue;
        if (4 == i) continue;

        for (int j = 0; j < MAX_POSSIBLE_MOVES; ++j) {
            /* If the destination slot is occupied, the move cannot be completed. */
            if (EMPTY != current_state->s[move[i].destinations[j]]) continue;

            /* Create a new board state from the expansion. */
            board_t *new_state = calloc(1, sizeof(board_t));
            memcpy(new_state, current_state, sizeof(board_t));

            /* Move the piece by getting what the old space held
             * and applying it to the new space. The old space becomes EMPTY. */
            board_space_state_t old_state = new_state->s[i];
            new_state->s[i] = EMPTY;
            new_state->s[move[i].destinations[j]] = old_state;

            /* Track the parent state we expanded from. */
            new_state->parent_state = current_state;
            new_state->moves_from_start = current_state->moves_from_start + 1;

            /* Make sure this new possible state has not already been visited. */
            unsigned int state_code = get_state_code(new_state);
            if (0 == list__contains(game->visited_boards,
                                    &state_code,
                                    sizeof(unsigned int))) {
                /* Notice how B&B is not checking whether a sub-tree was already expanded. */
                debug("\nDiscovered new possible move:\n");
                print_board(new_state);

                /*
                 * F(x) is the total cost estimate.
                 * G(x) is the amount of moves away from the origin for this expansion.
                 * H(x) is not defined with branch and bound.
                 */
                unsigned int h_x = 0;
                unsigned int g_x = new_state->moves_from_start;
                unsigned int f_x = g_x + h_x;

                debug("Stats:\tf(x) = %u\tg(x) = %u\th(x) = %u\n", f_x, g_x, h_x);

                queue__insert(game->priority_queue,
                              new_state,
                              f_x,
                              g_x,
                              h_x);

                /* Track this board state as 'visited' in BnB. */
                unsigned int *state = (unsigned int *)malloc(sizeof(unsigned int));
                *state = get_state_code(new_state);
                list__insert(game->visited_boards, (node_t)state);
            }
        }
    }
}



/* Main program method. Run simulations and report. */
int
main()
{
    clock_t start, end;
    double astar_time, bnb_time;
    unsigned int astar_expansions, bnb_expansions;
    game_t _four_knights = {
        .current_board_state = (board_t *)calloc(1, sizeof(board_t)),
        .initial_board_state = {
            .a1 = BLACK_1,
            .a2 = EMPTY,
            .a3 = BLACK_2,
            .b1 = EMPTY,
            .b2 = EMPTY,
            .b3 = EMPTY,
            .c1 = WHITE_1,
            .c2 = EMPTY,
            .c3 = WHITE_2,
            .moves_from_start = 0,
            .parent_state = NULL
        },
        .goal_board_state = {
            .a1 = WHITE_2,
            .a2 = EMPTY,
            .a3 = WHITE_1,
            .b1 = EMPTY,
            .b2 = EMPTY,
            .b3 = EMPTY,
            .c1 = BLACK_2,
            .c2 = EMPTY,
            .c3 = BLACK_1
        },
    };

    /* This is stupid, but w/e. */
    four_knights = calloc(sizeof(game_t), 1);
    memcpy(four_knights, &_four_knights, sizeof(game_t));

    /* OK, start the simulations. */
    debug("\n\n=~=~= Four Knights Puzzle Simulator =~=~=\n\n");
    debug("\n-- Initializing game board...\n");
    reset_game(four_knights);
    print_board(four_knights->current_board_state);
    debug( "\n-- Game goal state...\n");
    print_board(&four_knights->goal_board_state);


    /**************************************************************
     *
     *    A-Star...
     *
     **************************************************************/
    debug("\n-- Running A* Search for best solution...\n");
    start = clock();
    while (0 != check_game(four_knights)) {
        /* Increase the counter of times we've expanded tree nodes. */
        ++four_knights->expansions;

        /* Add the next set of moves to the search list. */
        astar__get_next_possible_moves(four_knights);

        /* Select the lowest-cost path according to the set of expanded moves. */
        queue_object_t queue_obj = queue__get_min(four_knights->priority_queue);
        if (NULL == queue_obj.item) {
            fprintf(stderr, "Uh oh! Looks like there are no more possibilities.\n");
            exit(1);
        }

        /* Set the current board state to the plucked entry. */
        four_knights->current_board_state = queue_obj.item;

        /* Track this board state as 'visited'. */
        unsigned int *state = (unsigned int *)malloc(sizeof(unsigned int));
        *state = get_state_code((board_t *)queue_obj.item);
        list__insert(four_knights->visited_boards, (node_t)state);

        /* Print out the route selection for expansion. */
        debug("\n === Selected Route w/ Cost %d ===\n", queue_obj.F);
        print_board(four_knights->current_board_state);
    }

    /* Print a post-op summary. */
    end = clock();
    astar_expansions = four_knights->expansions;
    astar_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    print_final_game_solution(four_knights);
    debug("\n==*=*=*=*=*=*=*=*=*=*=*==\nNice! You won!!!\n");
    debug("\tTree Expansions with A*: %u\n\tTime taken: %f seconds\n\n",
           astar_expansions,
           astar_time);


    /**************************************************************
     *
     *    BRANCH AND BOUND...
     *      No extended list filtering. Exhaustive.
     *
     **************************************************************/
    /* Reset the game and go for branch and bound searching. */
    debug("\n\n\n========================================\nPlaying the game with branch and bound...\n");
    debug("\n-- Initializing game board...\n");
    reset_game(four_knights);
    print_board(four_knights->current_board_state);
    debug( "\n-- Game goal state...\n");
    print_board(&four_knights->goal_board_state);

    start = clock();
    while (0 != check_game(four_knights)) {
        /* Increase the counter of times we've expanded tree nodes. */
        ++four_knights->expansions;

        /* Add the next set of moves to the search list. */
        bnb__get_next_possible_moves(four_knights);

        /* Select the lowest-cost path according to the set of expanded moves. */
        queue_object_t queue_obj = queue__get_min(four_knights->priority_queue);
        if (NULL == queue_obj.item) {
            fprintf(stderr, "Uh oh! Somehow got a NULL lowest cost.\n");
            exit(1);
        }

        /* Set the current board state to the plucked entry. */
        four_knights->current_board_state = queue_obj.item;

        /* Print out the route selection for expansion. */
        debug("\n === Selected Route w/ Cost %d ===\n", queue_obj.F);
        print_board(four_knights->current_board_state);
    }

    /* Print a post-op summary. */
    end = clock();
    bnb_expansions = four_knights->expansions;
    bnb_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    print_final_game_solution(four_knights);
    debug("\n==*=*=*=*=*=*=*=*=*=*=*==\nNice! You won!!!\n");
    debug("\tTree Expansions with B&B: %u\n\tTime taken: %f seconds\n\n",
           bnb_expansions,
           bnb_time);

    /* All done! */
    PRINT("\nType, Time (microseconds), Expansions\n");
    PRINT("A-Star, %f, %u\n", astar_time * 1000 * 1000, astar_expansions);
    PRINT("Branch and Bound, %f, %u\n", bnb_time * 1000 * 1000, bnb_expansions);
    reset_game(four_knights);
    return 0;
}
