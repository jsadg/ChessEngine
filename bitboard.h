#ifndef BITBOARD_H
#define BITBOARD_H

#include <stdint.h>

// 64-bit Board definition
#define U64 unsigned long long

// Debruijn board table for ls1b calculation
extern const int index64[64];

// Bit counting function
static inline int count_bits(U64 bitboard){
    return __builtin_popcountll(bitboard);
}

// Returns the index of the lowest 'on' bit
static inline int get_ls1b_index(U64 bitboard){
    if (!bitboard){
        return -1;
    }
    U64 isolated = bitboard & -bitboard;
    U64 debruijn = (isolated * 0x03f79d71b4cb0a89ULL) >> 58;
    return index64[debruijn];
}

// Bitboard macros for manipulation
#define set_bit(bitboard, square) ((bitboard) |= (1ULL << (U64)(square)))
#define get_bit(bitboard, square) ((bitboard) & (1ULL << (U64)(square)))
#define rem_bit(bitboard, square) ((bitboard) &= ~(1ULL << (U64)(square)))

#endif
