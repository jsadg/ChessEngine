#ifndef EVAL_H
#define EVAL_H

#include "bitboard.h"
#include "board.h"
#include "math.h"
#include "attacks.h"

// Psqt tables for different parts of the game
extern int middlegame_piece_square_total[12][64];

extern int endgame_piece_square_total[12][64];

// Returns 0 if middle game, 1 if endgame, threshold based on rough value of pieces
static inline int get_game_state(){
    int material = 
        9 * (count_bits(bitboards[Q]) + count_bits(bitboards[q])) +
        5 * (count_bits(bitboards[R]) + count_bits(bitboards[r])) +
        3 * (count_bits(bitboards[B]) + count_bits(bitboards[b])) +
        3 * (count_bits(bitboards[N]) + count_bits(bitboards[n]));

    if(material > 140){
        return 0;
    }
    else{
        return 1;
    }
}

// Convert evals and positional scores to piece_square_total for middlegame
void init_middlegame_psqt();

// Convert evals and positional scores to piece_square_total for endgame
void init_endgame_psqt();

// Calculates pawn structure penalties for the position
int calc_pawn_structure();

// Calculates the mobility bonus for each piece in the position
int calc_piece_mobility();

// Eval bonuses such as bishop pair/open files
int calc_piece_bonuses();

// Calculate king safety bonuses/penalties based on pawn shield and piece defenders
int calc_king_safety();

// Return the eval of the current position
int evaluate();

#endif
