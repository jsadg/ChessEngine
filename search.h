#ifndef SEARCH_H
#define SEARCH_H

#include "bitboard.h"
#include "board.h"
#include "movegen.h"
#include "io.h"
#include "eval.h"
#include <stdlib.h>

// Table for most valuable victim, least valuable attacker
extern int mvv_lva_table[6][6];



// Search capture/promotion related moves to avoid horizon effect
int quiescence(int alpha, int beta);

// Returns the sorting score for a move
int get_move_score(int move);

// Comparison function for qsort_r
int compare_moves(const void *a, const void *b);

// Sorts the move list via qsort and move_scores
void sort_moves(moves *move_list);

// Alpha beta loop to evaluate at a depth
int negamax(int alpha, int beta, int depth);

// Search the current position at the given depth
void search_position(int depth);

#endif
