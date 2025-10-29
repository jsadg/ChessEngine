#ifndef SEARCH_H
#define SEARCH_H

#include "bitboard.h"
#include "board.h"
#include "movegen.h"

// Table of material scores
extern const int material_score[12];

// Positional scores based on where the piece is
extern const int pawn_score[64];
extern const int knight_score[64];
extern const int bishop_score[64];
extern const int rook_score[64];
extern const int queen_score[64];
extern const int king_score[64];

// Used to mirror positional eval
const int mirror_score[128];

// Search the current position at the given depth
char* search_position(int depth);

// Return the eval of the current position
int evaluate();




#endif
