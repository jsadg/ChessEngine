#ifndef ATTACKS_H
#define ATTACKS_H

#include "bitboard.h"
#include "board.h"
#include "magics.h"

// Pawn attack table [side][square]
extern U64 pawn_attacks[2][64];

// Knight attack table [square]
extern U64 knight_attacks[64];

// King attack table [square]
extern U64 king_attacks[64];

// Bishop masks and attack tables
extern U64 bishop_masks[64];
extern U64 bishop_attacks[64][512];

// Rook masks and attack tables
extern U64 rook_masks[64];
extern U64 rook_attacks[64][4096];

// Pawn attacks
U64 mask_pawn_attacks(int side, int square);
void init_pawn_attack_table(void);

// Knight attacks
U64 mask_knight_attacks(int square);
void init_knight_attack_table(void);

// King attacks
U64 mask_king_attacks(int square);
void init_king_attack_table(void);

// Bishop attacks
U64 mask_bishop_attacks(int square);
U64 bishop_attacks_game(int square, U64 block);
void init_bishop_attacks(void);

// Bishop attacks getter
static inline U64 get_bishop_attacks(int square, U64 occupancy){
    occupancy &= bishop_masks[square];
    occupancy *= bishop_magics[square];
    occupancy >>= 64-bishop_bits[square];
    return bishop_attacks[square][occupancy];
}

// Rook attacks
U64 mask_rook_attacks(int square);
U64 rook_attacks_game(int square, U64 block);
void init_rook_attacks(void);

// Rook attacks getter
static inline U64 get_rook_attacks(int square, U64 occupancy){
    occupancy &= rook_masks[square];
    occupancy *= rook_magics[square];
    occupancy >>= 64-rook_bits[square];
    return rook_attacks[square][occupancy];
}

// Queen attacks getter(combined rook + bishop)
static inline U64 get_queen_attacks(int square, U64 occupancy){
    U64 queen_attacks = 0ULL;
    U64 bishop_occupancy = occupancy;
    U64 rook_occupancy = occupancy;

    // Get bishop attacks
    bishop_occupancy &= bishop_masks[square];
    bishop_occupancy *= bishop_magics[square];
    bishop_occupancy >>= 64-bishop_bits[square];
    queen_attacks = bishop_attacks[square][bishop_occupancy];
    
    // Get rook attacks
    rook_occupancy &= rook_masks[square];
    rook_occupancy *= rook_magics[square];
    rook_occupancy >>= 64-rook_bits[square];
    queen_attacks |= rook_attacks[square][rook_occupancy];

    return queen_attacks;
}

// Grab occupancy from inputted attack mask
U64 set_occupancy(int index, int bits_in_mask, U64 attack_mask);

// Initialize all attack tables
void init_piece_attack_tables(void);

#endif
