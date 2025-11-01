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
    15,   5,  15,  35,  35,  15,   5,  15,
    10,   0,  10,  30,  30,   5,   0,  10,
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

// Best move in the position
int best_move;

// Half move
int ply = 0;

int quiescence(int alpha, int beta){
    // Evaluate position
    int evaluation = evaluate();
    
    // Beta cutoff
    if(evaluation >= beta){
        // Node fails high
        return beta;
    }

    // Better move
    if(evaluation > alpha){
        alpha = evaluation;
    }

    moves move_list;
    generate_moves(&move_list);
    prune_to_quiescence(&move_list);
    for(int count = 0; count < move_list.count; count++){
        ply++;
        // If move is illegal take it back
        if(make_move(move_list.moves[count]) == 0){
            ply--;
            continue;
        }
        // Recursively call negamax with negative parameters
        int score = -quiescence(-beta, -alpha);

        take_back();
        ply--;
        
        // Beta cutoff
        if(score >= beta){
            // Node fails high
            return beta;
        }

        // Better move
        if(score > alpha){
            alpha = score;
        }
    }
    // Move fails low
    return alpha;
}

int negamax(int alpha, int beta, int depth){
    if(depth == 0){
        return quiescence(alpha, beta);
    }
    int in_check = is_square_attacked((side == white) ? get_ls1b_index(bitboards[K]) : get_ls1b_index(bitboards[k]), side ^ 1);
    int legal_moves = 0;
    int best_sofar = 0;
    int old_alpha = alpha;

    // Generate moves for move list
    moves move_list;
    generate_moves(&move_list);
    for(int count = 0; count < move_list.count; count++){
        ply++;
        // If move is illegal take it back
        if(make_move(move_list.moves[count]) == 0){
            ply--;
            continue;
        }
        legal_moves++;
        // Recursively call negamax with negative parameters
        int score = -negamax(-beta, -alpha, depth-1);
        take_back();
        ply--;
        
        // Beta cutoff
        if(score >= beta){
            // Node fails high
            return beta;
        }

        // Better move
        if(score > alpha){
            alpha = score;
            if(ply == 0){
                best_sofar = move_list.moves[count];
            }
        }
    }
    // Detect checkmate
    if(legal_moves == 0){
        if(in_check){
            return -49000 + ply;
        }
        else{ // Stalemate
            return 0;
        }
    }

    if(old_alpha != alpha){
        best_move = best_sofar;
    }
    // Move fails low
    return alpha;
}

// Find best move given a position
void search_position(int depth){
    ply = 0;
    best_move = 0;
    pos_num = 0;
    negamax(-50000, 50000, depth);
    printf("bestmove ");
    print_move(best_move);
    printf("\n");
}

