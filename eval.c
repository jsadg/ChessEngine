#include "eval.h"

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
    20,  20,  20,  37,  37,  20,  20,  20,
    15,   5,  15,  35,  35,  15,   5,  15,
    10,   0,  10,  32,  32,   5,   0,  10,
     5,   0,   0,   5,   5,  -5,   5,   5,
     5,   5,   5, -10, -10,   5,   5,   5,
     0,   0,   0,   0,   0,   0,   0,   0
};

// Corners and edge bad, motivation to develop
const int knight_score[64] = {
   -20, -10, -10, -10, -10, -10, -10, -20,
   -10,  -5,   0,   0,   0,   0,  -5, -10,
   -10,   0,  20,  30,  30,  20,   0, -10,
   -10,   0,  25,  30,  30,  25,   0, -10,
   -10,   0,  25,  30,  30,  25,   0, -10,
   -10,   0,  20,   5,   5,  20,   0, -10,
   -10,  -5,   0,  10,  10,   0,  -5, -10,
   -20, -12, -10, -10, -10, -10, -12, -20
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

// Home area ok, center ok, main diagonal ok
const int queen_score[64] = {
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   5,   5,   5,   5,   0,   0,
     5,   0,   5,  10,  10,   5,   0,   0,
     0,  15,   0,  15,  15,   0,   0,   0,
     0,   0,  15,  15,  15,   0,   0,   0,
     0,   0,   0,  15,   0,   0,   0,   0
};

// Everywhere except castled bad
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

int piece_square_total[12][64];

// Used to flip for black
static inline int mirror_square(int square) { return square ^ 56; }


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
                    pos_bonus = -pawn_score[mirror_square(square)];
                    break;
                case n:
                    pos_bonus = -knight_score[mirror_square(square)];
                    break;
                case b:
                    pos_bonus = -bishop_score[mirror_square(square)];
                    break;
                case r:
                    pos_bonus = -rook_score[mirror_square(square)];
                    break;
                case q:
                    pos_bonus = -queen_score[mirror_square(square)];
                    break;
                case k:
                    pos_bonus = -king_score[mirror_square(square)];
                    break;
            }
            piece_square_total[piece][square] = material_score[piece] + pos_bonus;
        }
    }
}

int calc_pawn_structure(){
    // Get file of all white pawns
    int white_pawn_array[8] = {0};
    U64 bitboard = bitboards[P];
    int square = 0;

    // Calculate isolated pawns
    U64 left = (bitboard << 1) & not_a_file;
    U64 right = (bitboard >> 1) & not_h_file;
    int white_isolated = count_bits(bitboard & ~(left | right));

    while(bitboard){
        square = get_ls1b_index(bitboard);
        white_pawn_array[square % 8]++;
        rem_bit(bitboard, square);
    }
    int white_doubled = 0;
    // Calculate number of doubled pawns
    for(int i = 0; i < 8; i++){
        if(white_pawn_array[i] >= 2){
            white_doubled += (white_pawn_array[i]-1);
        }
    }
    int white_islands = 0;
    // Calculate pawn islands
    for(int i = 0; i < 8; i++){
        if(white_pawn_array[i] != 0){
            white_islands++;
            while(i < 8 && white_pawn_array[i] != 0){
                i++;
            }
        }
    }

    int black_pawn_array[8] = {0};
    bitboard = bitboards[p];

    // Calculate isolated pawns
    left = (bitboard << 1) & not_a_file;
    right = (bitboard >> 1) & not_h_file;
    int black_isolated = count_bits(bitboard & ~(left | right));

    while(bitboard){
        square = get_ls1b_index(bitboard);
        black_pawn_array[square % 8]++;
        rem_bit(bitboard, square);
    }
    int black_doubled = 0;
    // Calculate number of doubled pawns
    for(int i = 0; i < 8; i++){
        if(black_pawn_array[i] >= 2){
            black_doubled += (black_pawn_array[i]-1);
        }
    }
    int black_islands = 0;
    // Calculate pawn islands
    for(int i = 0; i < 8; i++){
        if(black_pawn_array[i] != 0){
            black_islands++;
            while(i < 8 && black_pawn_array[i] != 0){
                i++;
            }
        }
    }

    int score = 0;

    // Reverse of score since these are penalties    
    score -= (white_doubled*10) + (white_islands * white_islands * 2) + (white_isolated * 13);
    score += (black_doubled*10) + (black_islands * black_islands * 2) + (black_isolated * 13);
    return score;
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
    // Add pawn structure calculation
    score += calc_pawn_structure();

    return (side == white) ? score : -score;
}
