#ifndef SEARCH_H
#define SEARCH_H

#include "bitboard.h"
#include "board.h"
#include "movegen.h"
#include "io.h"

// Table of material scores
extern const int material_score[12];

// Positional scores based on where the piece is
extern const int pawn_score[64];
extern const int knight_score[64];
extern const int bishop_score[64];
extern const int rook_score[64];
extern const int queen_score[64];
extern const int king_score[64];

extern int piece_square_total[12][64];

// Convert evals and positional scores to piece_square_total for lookup
void init_psqt();

// Search the current position at the given depth
void search_position(int depth);

// Return the eval of the current position
int evaluate();

// Alpha beta loop to evaluate at a depth
int negamax(int alpha, int beta, int depth);


#endif
