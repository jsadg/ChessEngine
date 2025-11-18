#include "search.h"

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
    // Used for checkmate detection
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
            // Move is a root
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
        // Stalemate
        else{
            return 0;
        }
    }
    // If change in alpha then best_move changes
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

