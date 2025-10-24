#include "bitboard.h"
#include "board.h"
#include "attacks.h"
#include "movegen.h"
#include <stdio.h>

BoardState board_stack[MAX_PLY];
int ply = 0;

char promoted_pieces[] = {
    [Q] = 'q',
    [R] = 'r',
    [B] = 'b',
    [N] = 'n',
    [q] = 'q',
    [r] = 'r',
    [b] = 'b',
    [n] = 'n'
};

const int castling_rights[64] = {
    7,15,15,15,3,15,15,11,
    15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,
    13,15,15,15,12,15,15,14
};


int make_move(int move, int move_type){
    // Non-Captures

    if(move_type == all_moves){
        // Save board state
        copy_board();
        // Board variables
        int source_square = get_source(move);
        int target_square = get_target(move);
        int piece = get_piece(move);
        int promoted_piece = get_promoted(move);
        int capture = get_capture(move);
        int double_push = get_double(move);
        int ep = get_enpassant(move);
        int castling = get_castling(move);
        // Move piece
        rem_bit(bitboards[piece], source_square);
        if(promoted_piece){
            set_bit(bitboards[promoted_piece], target_square);
        }
        else{
            set_bit(bitboards[piece], target_square);
        }

        if(capture){
            int start_piece, end_piece;

            if(side == white){
                start_piece = p;
                end_piece = k;
            }
            else{
                start_piece = P;
                end_piece = K;
            }
            // Loop over the opposite side's bitboards to remove the captured piece
            for(int piece = start_piece; piece <= end_piece; piece++){
                // If piece on target square
                if(get_bit(bitboards[piece], target_square)){
                    // Remove the piece
                    rem_bit(bitboards[piece], target_square);
                    rem_bit(occupancies[!side], target_square);
                    break;
                }
            }
        }
        if(ep){
            // Remove pawn captured by e.p.
            (side == white) ? rem_bit(bitboards[p], target_square + 8) : rem_bit(bitboards[P], target_square - 8);
            (side == white) ? rem_bit(occupancies[black], target_square + 8) : rem_bit(occupancies[white], target_square - 8);

        }
        // Reset enpassant square after move
        enpassant = no_sq;

        // Make enpassant square available on double pawn pushes
        if(double_push){
            (side == white) ? (enpassant = target_square + 8) : (enpassant = target_square - 8);
        }

        if(castling){
            switch(target_square){
                // White kingside
                case(g1):
                    rem_bit(bitboards[R], h1);
                    set_bit(bitboards[R], f1);
                    rem_bit(occupancies[white], h1);
                    set_bit(occupancies[white], f1);
                    break;
                // White queenside
                case(c1):
                    rem_bit(bitboards[R], a1);
                    set_bit(bitboards[R], d1);
                    rem_bit(occupancies[white], a1);
                    set_bit(occupancies[white], d1);
                    break;
                // Black kingside
                case(g8):
                    rem_bit(bitboards[r], h8);
                    set_bit(bitboards[r], f8);
                    rem_bit(occupancies[black], h8);
                    set_bit(occupancies[black], f8);
                    break;
                // Black queenside
                case(c8):
                    rem_bit(bitboards[r], a8);
                    set_bit(bitboards[r], d8);
                    rem_bit(occupancies[black], a8);
                    set_bit(occupancies[black], d8);
                    break;
            }
        }
        // Update castling rights
        castle &= castling_rights[source_square];
        // Handles capturing of rooks
        castle &= castling_rights[target_square];

        // Handles occpancies updates for moves
        rem_bit(occupancies[side], source_square);
        set_bit(occupancies[side], target_square);

        occupancies[both] = occupancies[white] | occupancies[black];

        // Flip side after move to check for illegal moves
        side ^= 1;

        // Check if move places king in check
        if(is_square_attacked((side == white) ? get_ls1b_index(bitboards[k]) : get_ls1b_index(bitboards[K]), side)){
            take_back();
            // Illegal move
            return 0;
        }
        else{
            // Legal move
            return 1;
        }
    }
    // Captures
    else if(get_capture(move)){
        return make_move(move, all_moves);
    }
    else{
        return 0;
    }
}

void generate_moves(moves *move_list){
    move_list->count = 0;
    // Where a piece currently is
    int source_square;
    // Where it is heading
    int target_square;

    // Define a piece's bitboard copy and its attacks
    U64 bitboard;
    U64 attacks;

    for(int piece = P; piece <= k; piece++){
        // Get a copy of the piece bitboard
        bitboard = bitboards[piece];

        // White pawn move generation
        if(piece == P && side == white){
            U64 single_push = (bitboard >> 8) & (~occupancies[both]); 
            // Single pawn pushes
            while(single_push){
                target_square = get_ls1b_index(single_push);
                source_square = target_square + 8;
                // Pawn promotions
                if(target_square >= a8 && target_square <= h8){
                    add_move(move_list, encode_move(source_square, target_square, P, Q, 0, 0, 0, 0));
                    add_move(move_list, encode_move(source_square, target_square, P, R, 0, 0, 0, 0));
                    add_move(move_list, encode_move(source_square, target_square, P, B, 0, 0, 0, 0));
                    add_move(move_list, encode_move(source_square, target_square, P, N, 0, 0, 0, 0));
                }
                else{
                    add_move(move_list, encode_move(source_square, target_square, P, 0, 0, 0, 0, 0));
                    // Double pawn moves
                    if (source_square >= a2 && source_square <= h2) {
                        int double_push = source_square - 16;
                        if (!get_bit(occupancies[both], double_push))
                            add_move(move_list, encode_move(source_square, double_push, P, 0, 0, 1, 0, 0));
                    }
                }
                rem_bit(single_push, target_square); 
            }
            // Pawn attacks
            while(bitboard){
                source_square = get_ls1b_index(bitboard);
            
                attacks = pawn_attacks[white][source_square] & occupancies[black];
                while(attacks){
                    target_square = get_ls1b_index(attacks);
                    // Attacks with promotion
                    if(target_square >= a8 && target_square <= h8){
                        add_move(move_list, encode_move(source_square, target_square, P, Q, 1, 0, 0, 0));
                        add_move(move_list, encode_move(source_square, target_square, P, R, 1, 0, 0, 0));
                        add_move(move_list, encode_move(source_square, target_square, P, B, 1, 0, 0, 0));
                        add_move(move_list, encode_move(source_square, target_square, P, N, 1, 0, 0, 0));
                    }
                    else{
                        add_move(move_list, encode_move(source_square, target_square, P, 0, 1, 0, 0, 0));
                    }
                    rem_bit(attacks, target_square);
                }
                // Enpassant handler
                if(enpassant != no_sq){
                    U64 enpassant_attacks = pawn_attacks[white][source_square] & (1ULL << enpassant);
                    if(enpassant_attacks){
                        int target_enpassant = get_ls1b_index(enpassant_attacks);
                        add_move(move_list, encode_move(source_square, target_enpassant, P, 0, 1, 0, 1, 0));
                    }
                }
                rem_bit(bitboard, source_square);
            }
        }
        // Black pawn move generation
        if(piece == p && side == black){
            // Single pawn pushes
            U64 single_push = (bitboard << 8) & (~occupancies[both]);
            while(single_push){
                target_square = get_ls1b_index(single_push);
                source_square = target_square - 8;
                // Pawn promotions
                if(target_square >= a1 && target_square <= h1){
                    add_move(move_list, encode_move(source_square, target_square, p, q, 0, 0, 0, 0));
                    add_move(move_list, encode_move(source_square, target_square, p, r, 0, 0, 0, 0));
                    add_move(move_list, encode_move(source_square, target_square, p, b, 0, 0, 0, 0));
                    add_move(move_list, encode_move(source_square, target_square, p, n, 0, 0, 0, 0));
                }
                else{
                    add_move(move_list, encode_move(source_square, target_square, p, 0, 0, 0, 0, 0));
                    // Double pawn moves
                    if (source_square >= a7 && source_square <= h7) {
                        int double_push = source_square + 16;
                        if (!get_bit(occupancies[both], double_push))
                            add_move(move_list, encode_move(source_square, double_push, p, 0, 0, 1, 0, 0));
                    }
                }
                rem_bit(single_push, target_square);
            }
            while(bitboard){
                source_square = get_ls1b_index(bitboard);
                attacks = pawn_attacks[black][source_square] & occupancies[white];
                while(attacks){
                    target_square = get_ls1b_index(attacks);
                    // Attacks with promotion
                    if(target_square >= a1 && target_square <= h1){
                        add_move(move_list, encode_move(source_square, target_square, p, q, 1, 0, 0, 0));
                        add_move(move_list, encode_move(source_square, target_square, p, r, 1, 0, 0, 0));
                        add_move(move_list, encode_move(source_square, target_square, p, b, 1, 0, 0, 0));
                        add_move(move_list, encode_move(source_square, target_square, p, n, 1, 0, 0, 0));
                    }
                    else{
                        add_move(move_list, encode_move(source_square, target_square, p, 0, 1, 0, 0, 0));
                    }
                    rem_bit(attacks, target_square);
                }
                // Enpassant handler
                if(enpassant != no_sq){
                    U64 enpassant_attacks = pawn_attacks[black][source_square] & (1ULL << enpassant);
                    if(enpassant_attacks){
                        int target_enpassant = get_ls1b_index(enpassant_attacks);
                        add_move(move_list, encode_move(source_square, target_enpassant, p, 0, 1, 0, 1, 0));
                    }
                }
                rem_bit(bitboard, source_square);
            }
        }

        // White castling handler
        if(piece == K && side == white){
            // White kingside castling is legal
            if(castle & wkc){
                if(!get_bit(occupancies[both], f1) && !get_bit(occupancies[both], g1) && !is_square_attacked(f1, black) && !is_square_attacked(g1, black)){
                    add_move(move_list, encode_move(e1, g1, K, 0, 0, 0, 0, 1));
                }
            }
            // White queenside castling is legal
            if(castle & wqc){
                if(!get_bit(occupancies[both], d1) && !get_bit(occupancies[both], c1) && !is_square_attacked(d1, black) && !is_square_attacked(c1, black)){
                    add_move(move_list, encode_move(e1, c1, K, 0, 0, 0, 0, 1));
                }
            }
        }
        // Black castling handler
        if(piece == k && side == black){
            // Black kingside castling is legal
            if(castle & bkc){
                if(!get_bit(occupancies[both], f8) && !get_bit(occupancies[both], g8) && !is_square_attacked(f8, white) && !is_square_attacked(g8, white)){
                    add_move(move_list, encode_move(e8, g8, k, 0, 0, 0, 0, 1));
                }
            }
            // Black queenside castling is legal
            if(castle & bqc){
                if(!get_bit(occupancies[both], d8) && !get_bit(occupancies[both], c8) && !is_square_attacked(d8, white) && !is_square_attacked(c8, white)){
                    add_move(move_list, encode_move(e8, c8, k, 0, 0, 0, 0, 1));
                }
            }
        }

        // White knight move generation
        if(piece == N && side == white){
            while(bitboard){
                source_square = get_ls1b_index(bitboard);
                // Get knight attacks without landing on own piece
                attacks = knight_attacks[source_square] & ~occupancies[white];
                while(attacks){
                    target_square = get_ls1b_index(attacks);
                    // Non-capture
                    if(!get_bit(occupancies[black], target_square)){
                        add_move(move_list, encode_move(source_square, target_square, N, 0, 0, 0, 0, 0));
                    }
                    // Capture
                    else{
                        add_move(move_list, encode_move(source_square, target_square, N, 0, 1, 0, 0, 0));
                    }
                    rem_bit(attacks, target_square);
                }
                rem_bit(bitboard, source_square);
            }
        }
        // Black knight move generation
        if(piece == n && side == black){
            while(bitboard){
                source_square = get_ls1b_index(bitboard);
                // Get knight attacks without landing on own piece
                attacks = knight_attacks[source_square] & ~occupancies[black];
                while(attacks){
                    target_square = get_ls1b_index(attacks);
                    // Non-capture
                    if(!get_bit(occupancies[white], target_square)){
                        add_move(move_list, encode_move(source_square, target_square, n, 0, 0, 0, 0, 0));
                    }
                    // Capture
                    else{
                        add_move(move_list, encode_move(source_square, target_square, n, 0, 1, 0, 0, 0));
                    }
                    rem_bit(attacks, target_square);
                }
                rem_bit(bitboard, source_square);
            }
        }

        // White bishop move generation
        if(piece == B && side == white){
            while(bitboard){
                source_square = get_ls1b_index(bitboard);
                // Get bishop attacks without landing on own piece
                attacks = get_bishop_attacks(source_square, occupancies[both]) & ~occupancies[white];
                while(attacks){
                    target_square = get_ls1b_index(attacks);
                    // Non-capture
                    if(!get_bit(occupancies[black], target_square)){
                        add_move(move_list, encode_move(source_square, target_square, B, 0, 0, 0, 0, 0));
                    }
                    // Capture
                    else{
                        add_move(move_list, encode_move(source_square, target_square, B, 0, 1, 0, 0, 0));
                    }
                    rem_bit(attacks, target_square);
                }
                rem_bit(bitboard, source_square);
            }
        }
        // Black bishop move generation
        if(piece == b && side == black){
            while(bitboard){
                source_square = get_ls1b_index(bitboard);
                // Get bishop attacks without landing on own piece
                attacks = get_bishop_attacks(source_square, occupancies[both]) & ~occupancies[black];
                while(attacks){
                    target_square = get_ls1b_index(attacks);
                    // Non-capture
                    if(!get_bit(occupancies[white], target_square)){
                        add_move(move_list, encode_move(source_square, target_square, b, 0, 0, 0, 0, 0));
                    }
                    // Capture
                    else{
                        add_move(move_list, encode_move(source_square, target_square, b, 0, 1, 0, 0, 0));
                    }
                    rem_bit(attacks, target_square);
                }
                rem_bit(bitboard, source_square);
            }
        }

        // White rook move generation
        if(piece == R && side == white){
            while(bitboard){
                source_square = get_ls1b_index(bitboard);
                // Get rook attacks without landing on own piece
                attacks = get_rook_attacks(source_square, occupancies[both]) & ~occupancies[white];
                while(attacks){
                    target_square = get_ls1b_index(attacks);
                    // Non-capture
                    if(!get_bit(occupancies[black], target_square)){
                        add_move(move_list, encode_move(source_square, target_square, R, 0, 0, 0, 0, 0));
                    }
                    // Capture
                    else{
                        add_move(move_list, encode_move(source_square, target_square, R, 0, 1, 0, 0, 0));
                    }
                    rem_bit(attacks, target_square);
                }
                rem_bit(bitboard, source_square);
            }
        }
        // Black rook move generation
        if(piece == r && side == black){
            while(bitboard){
                source_square = get_ls1b_index(bitboard);
                // Get rook attacks without landing on own piece
                attacks = get_rook_attacks(source_square, occupancies[both]) & ~occupancies[black];
                while(attacks){
                    target_square = get_ls1b_index(attacks);
                    //Non-capture
                    if(!get_bit(occupancies[white], target_square)){
                        add_move(move_list, encode_move(source_square, target_square, r, 0, 0, 0, 0, 0));
                    }
                    //Capture
                    else{
                        add_move(move_list, encode_move(source_square, target_square, r, 0, 1, 0, 0, 0));
                    }
                    rem_bit(attacks, target_square);
                }
                rem_bit(bitboard, source_square);
            }
        }
        // White queen move generation
        if(piece == Q && side == white){
            while(bitboard){
                source_square = get_ls1b_index(bitboard);
                // Get queen attacks without landing on own piece
                attacks = get_queen_attacks(source_square, occupancies[both]) & ~occupancies[white];
                while(attacks){
                    target_square = get_ls1b_index(attacks);
                    // Non-capture
                    if(!get_bit(occupancies[black], target_square)){
                        add_move(move_list, encode_move(source_square, target_square, Q, 0, 0, 0, 0, 0));
                    }
                    // Capture
                    else{
                        add_move(move_list, encode_move(source_square, target_square, Q, 0, 1, 0, 0, 0));
                    }
                    rem_bit(attacks, target_square);
                }
                rem_bit(bitboard, source_square);
            }
        }
        // Black queen move generation
        if(piece == q && side == black){
            while(bitboard){
                source_square = get_ls1b_index(bitboard);
                // Get queen attacks without landing on own piece
                attacks = get_queen_attacks(source_square, occupancies[both]) & ~occupancies[black];
                while(attacks){
                    target_square = get_ls1b_index(attacks);
                    // Non-capture
                    if(!get_bit(occupancies[white], target_square)){
                        add_move(move_list, encode_move(source_square, target_square, q, 0, 0, 0, 0, 0));
                    }
                    // Capture
                    else{
                        add_move(move_list, encode_move(source_square, target_square, q, 0, 1, 0, 0, 0));
                    }
                    rem_bit(attacks, target_square);
                }
                rem_bit(bitboard, source_square);
            }
        }

        // White king move generation
        if(piece == K && side == white){
            while(bitboard){
                source_square = get_ls1b_index(bitboard);
                // Get king attacks without landing on own piece
                attacks = king_attacks[source_square] & ~occupancies[white];
                while(attacks){
                    target_square = get_ls1b_index(attacks);
                    // Non-capture
                    if(!get_bit(occupancies[black], target_square)){
                        add_move(move_list, encode_move(source_square, target_square, K, 0, 0, 0, 0, 0));
                    }
                    // Capture
                    else{
                        add_move(move_list, encode_move(source_square, target_square, K, 0, 1, 0, 0, 0));
                    }
                    rem_bit(attacks, target_square);
                }
                rem_bit(bitboard, source_square);
            }
        }

        // White king move generation
        if(piece == k && side == black){
            while(bitboard){
                source_square = get_ls1b_index(bitboard);
                // Get king attacks without landing on own piece
                attacks = king_attacks[source_square] & ~occupancies[black];
                while(attacks){
                    target_square = get_ls1b_index(attacks);
                    // Non-capture
                    if(!get_bit(occupancies[white], target_square)){
                        add_move(move_list, encode_move(source_square, target_square, k, 0, 0, 0, 0, 0));
                    }
                    // Capture
                    else{
                        add_move(move_list, encode_move(source_square, target_square, k, 0, 1, 0, 0, 0));
                    }
                    rem_bit(attacks, target_square);
                }
                rem_bit(bitboard, source_square);
            }
        }
    }
}