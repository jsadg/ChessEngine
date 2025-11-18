#ifndef SEARCH_H
#define SEARCH_H

#include "bitboard.h"
#include "board.h"
#include "movegen.h"
#include "io.h"
#include "eval.h"

// Search the current position at the given depth
void search_position(int depth);

// Search capture/promotion related moves to avoid horizon effect
int quiescence(int alpha, int beta);

// Alpha beta loop to evaluate at a depth
int negamax(int alpha, int beta, int depth);


#endif
