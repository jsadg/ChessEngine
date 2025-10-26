#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "bitboard.h"
#include "board.h"
#include "attacks.h"
#include <string.h>

// Movelist structure
typedef struct{
    // Max is ~218
    int moves[256];
    // Number of moves
    int count;
} moves;

// Board state structure
typedef struct {
    U64 bitboards[12];
    U64 occupancies[3];
    int side;
    int enpassant;
    int castle;
} BoardState;

// Max number of board states
#define MAX_PLY 256
extern BoardState board_stack[MAX_PLY];
extern int ply;

// Copy the given board
#define copy_board() \
    board_stack[ply].side = side; \
    board_stack[ply].enpassant = enpassant; \
    board_stack[ply].castle = castle; \
    memcpy(board_stack[ply].bitboards, bitboards, sizeof(bitboards)); \
    memcpy(board_stack[ply].occupancies, occupancies, sizeof(occupancies)); \
    ply++;

// Put back the copied board
#define take_back() \
    ply--; \
    side = board_stack[ply].side; \
    enpassant = board_stack[ply].enpassant; \
    castle = board_stack[ply].castle; \
    memcpy(bitboards, board_stack[ply].bitboards, sizeof(bitboards)); \
    memcpy(occupancies, board_stack[ply].occupancies, sizeof(occupancies)); \


/*                  Encoding macros
                binary representation                   hex constants
  0000 0000 0000 0000 0011 1111    source square        0x3f
  0000 0000 0000 1111 1100 0000    target square        0xfc0
  0000 0000 1111 0000 0000 0000    piece                0xf000
  0000 1111 0000 0000 0000 0000    promoted piece       0xf0000
  0001 0000 0000 0000 0000 0000    capture flag         0x100000
  0010 0000 0000 0000 0000 0000    double push flag     0x200000
  0100 0000 0000 0000 0000 0000    enpassant flag       0x400000
  1000 0000 0000 0000 0000 0000    castling flag        0x800000
*/

#define encode_move(source, target, piece, promoted, capture, double_push, enpassant, castling) \
((source) | ((target) << 6) | ((piece) << 12) | ((promoted) << 16) | ((capture) << 20) | ((double_push) << 21) | ((enpassant) << 22) | ((castling) << 23))

#define get_source(move)       ((move) & 0x3f)
#define get_target(move)       (((move) & 0xfc0) >> 6)
#define get_piece(move)        (((move) & 0xf000) >> 12)
#define get_promoted(move)     (((move) & 0xf0000) >> 16)
#define get_capture(move)      (((move) & 0x100000) >> 20)
#define get_double(move)       (((move) & 0x200000) >> 21)
#define get_enpassant(move)    (((move) & 0x400000) >> 22)
#define get_castling(move)     (((move) & 0x800000) >> 23)


// Type of move enum for quiesence search
enum { all_moves, captures };

// Castling rights lookup table
extern const int castling_rights[64];

// Promoted pieces character lookup
extern char promoted_pieces[];

// Function to add a move to a given movelist
static inline void add_move(moves *move_list, int move) {
    move_list->moves[move_list->count] = move;
    move_list->count++;
}


// Returns 1 if the given square is attacked by the given side
static inline int is_square_attacked(int square, int side){
    // Attacked by white pawns by reversing black pawn attacks
    if((side == white) && (pawn_attacks[black][square] & bitboards[P])){
        return 1;
    }
    // Attacked by black pawns by reversing white pawn attacks
    if((side == black) && (pawn_attacks[white][square] & bitboards[p])){
        return 1;
    }
    // Attacked by knights by reversing opposite color
    if(knight_attacks[square] & ((side == white) ? bitboards[N] : bitboards[n])){
        return 1;
    }
    // Attacked by bishops by reversing opposite color
    if(get_bishop_attacks(square, occupancies[both]) & ((side == white) ? bitboards[B] : bitboards[b])){
        return 1;
    }
    // Attacked by rooks by reversing opposite color
    if(get_rook_attacks(square, occupancies[both]) & ((side == white) ? bitboards[R] : bitboards[r])){
        return 1;
    }
    // Attacked by queens by reversing opposite color
    if(get_queen_attacks(square, occupancies[both]) & ((side == white) ? bitboards[Q] : bitboards[q])){
        return 1;
    }
    // Attacked by kings by reversing opposite color
    if(king_attacks[square] & ((side == white) ? bitboards[K] : bitboards[k])){
        return 1;
    }
    // Default not attacked
    return 0;
}

// Attempts to make a move; returns 1 if legal, 0 if illegal
int make_move(int move, int move_type);

// Generates all possible moves for the current position
void generate_moves(moves *move_list);

#endif
