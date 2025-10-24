#ifndef MAGIC_H
#define MAGIC_H

#include <stdint.h>
#include "bitboard.h"


// Rook magic numbers sourced from ChessProgramming
extern const U64 rook_magics[64];

// Bishop magic numbers sourced from ChessProgramming
extern const U64 bishop_magics[64];

// Rook and bishop relevant occupancy bit counts
extern const int rook_bits[64];
extern const int bishop_bits[64];

#endif
