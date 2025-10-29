#include "search.h"

const int material_score[12] = {
    100,    // White pawn
    325,    // White knight
    350,    // White bishop
    500,    // White rook
    950,    // White queen
    10000,  // White king
    -100,   //Black pawn
    -325,   // Black knight
    -350,   // Black bishop
    -500,   // Black rook
    -950,   // Black queen
    -10000  // Black king
};

// Space/center gaining moves good, promotion good, king safety good
const int pawn_score[64] = {
    80,  80,  80,  80,  80,  80,  80,  80,
    40,  40,  40,  40,  40,  40,  40,  40,
    20,  20,  20,  30,  30,  20,  20,  20,
    15,   5,  15,  30,  30,  15,   5,  15,
    10,   0,  10,  20,  20,   5,   0,  10,
     5,   0,   0,   5,   5,  -5,   5,   5,
     5,   5,   5,  -9,  -9,   5,   5,   5,
     0,   0,   0,   0,   0,   0,   0,   0
};

// Corners and edge bad, motivation to develop
const int knight_score[64] = {
   -20, -10, -10, -10, -10, -10, -10, -20,
   -10,  -5,   0,   0,   0,   0,  -5, -10,
   -10,   0,  20,  40,  40,  20,   0, -10,
   -10,   0,  30,  40,  40,  30,   0, -10,
   -10,   0,  30,  40,  40,  30,   0, -10,
   -10,   0,  20,   5,   5,  20,   0, -10,
   -10,  -5,   0,  10,  10,   0,  -5, -10,
   -20, -15, -10, -10, -10, -10, -15, -20
};
// Corners bad, own side good, motivation to develop
const int bishop_score[64] = {
   -10,   0,   0,   0,   0,   0,   0,  -10,
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,
     0,  15,   0,   0,   0,   0,  15,   0,
     0,   0,  20,   0,   0,  20,   0,   0,
     0,  20,   0,  20,  20,   0,  15,   0,
     5,  20,   0,  20,  20,   0,  20,   5,
   -10,   0,  -5,   0,   0,  -5,   0,  -10
};

// Central files good, 7th and 8th ranks good
const int rook_score[64] = {
    30,  30,  30,  30,  30,  30,  30,  30,
    50,  50,  50,  50,  50,  50,  50,  50,
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,
    -5,   0,  20,  30,  30,  20,   0,   0
};
const int queen_score[64] = {
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,  10,  10,   0,   0,   0,
     0,   0,   0,  10,  10,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,  15,  15,   0,   0,   0,
     0,   0,   0,  20,   0,   0,   0,   0
};
const int king_score[64] = {
   -20, -20, -20, -20, -20, -20, -20, -20,
   -20, -20, -20, -20, -20, -20, -20, -20,
   -20, -20, -20, -20, -20, -20, -20, -20,
   -20, -20, -20, -20, -20, -20, -20, -20,
   -20, -20, -20, -20, -20, -20, -20, -20,
   -20, -20, -20, -20, -20, -20, -20, -20,
     0,   0, -15, -15, -15, -15,   0,   0,
     5,  25,  30, -10, -10,  -5,  40,   5 
};

const int mirror_score[128] = {
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8,
};


int piece_square_total[12][64];

void init_psqt(){
    for(int piece = P; piece <= k; piece++){
        int pos_bonus;
        for(int square = 0; square < 64; square++){
            switch(piece){
                case P:
                    pos_bonus = pawn_score[square];
                    break;
                case N:
                    pos_bonus = knight_score[square];
                    break;
                case B:
                    pos_bonus = bishop_score[square];
                    break;
                case R:
                    pos_bonus = rook_score[square];
                    break;
                case Q:
                    pos_bonus = queen_score[square];
                    break;
                case K:
                    pos_bonus = king_score[square];
                    break;
                case p:
                    pos_bonus = -pawn_score[mirror_score[square]];
                    break;
                case n:
                    pos_bonus = -knight_score[mirror_score[square]];
                    break;
                case b:
                    pos_bonus = -bishop_score[mirror_score[square]];
                    break;
                case r:
                    pos_bonus = -rook_score[mirror_score[square]];
                    break;
                case q:
                    pos_bonus = -queen_score[mirror_score[square]];
                    break;
                case k:
                    pos_bonus = -king_score[mirror_score[square]];
                    break;
            }
            piece_square_total[piece][square] = material_score[piece] + pos_bonus;
        }
    }
}


int evaluate(){
    int score = 0;
    U64 bitboard;
    int square;
    // Loop through pieces and get eval from each
    for(int piece = P; piece <= k; piece++){
        bitboard = bitboards[piece];
        while(bitboard){
            square = get_ls1b_index(bitboard);
            // Piece weights
            score += piece_square_total[piece][square];
            rem_bit(bitboard, square);
        }
    }
    return (side == white) ? score : -score;
}


char* search_position(int depth){
    
    return "e2e4";
}
