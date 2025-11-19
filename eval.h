#ifndef EVAL_H
#define EVAL_H

#include "bitboard.h"
#include "board.h"
#include "math.h"
#include "attacks.h"

// Table of material scores
extern const int material_score[12];

// Positional scores based on where the piece is
extern const int pawn_score[64];
extern const int knight_score[64];
extern const int bishop_score[64];
extern const int rook_score[64];
extern const int queen_score[64];
extern const int king_score[64];

// Updated positional scores for endgames
extern const int pawn_endgame_score[64];
extern const int queen_endgame_score[64];
extern const int king_endgame_score[64];


extern int middlegame_piece_square_total[12][64];

extern int endgame_piece_square_total[12][64];


// Convert evals and positional scores to piece_square_total for middlegame
void init_middlegame_psqt();

// Convert evals and positional scores to piece_square_total for endgame
void init_endgame_psqt();

// Calculates pawn structure penalties for the position
int calc_pawn_structure();

// Calculates the mobility bonus for each piece in the position
int calc_piece_mobility();

// Return the eval of the current position
int evaluate();

#endif
