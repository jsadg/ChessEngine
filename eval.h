#ifndef EVAL_H
#define EVAL_H

#include "bitboard.h"
#include "board.h"
#include "math.h"

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

// Calculates pawn structure penalties for the position
int calc_pawn_structure();

// Return the eval of the current position
int evaluate();

#endif
