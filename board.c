#include "board.h"

const char *square_to_coords[] = {
    "a8","b8","c8","d8","e8","f8","g8","h8",
    "a7","b7","c7","d7","e7","f7","g7","h7",
    "a6","b6","c6","d6","e6","f6","g6","h6",
    "a5","b5","c5","d5","e5","f5","g5","h5",
    "a4","b4","c4","d4","e4","f4","g4","h4",
    "a3","b3","c3","d3","e3","f3","g3","h3",
    "a2","b2","c2","d2","e2","f2","g2","h2",
    "a1","b1","c1","d1","e1","f1","g1","h1"
};

// Capital for white, lowercase for black
char ascii_pieces[12] = "PNBRQKpnbrqk";

int char_pieces[] = {
    ['P'] = P,
    ['N'] = N,
    ['B'] = B,
    ['R'] = R,
    ['Q'] = Q,
    ['K'] = K,
    ['p'] = p,
    ['n'] = n,
    ['b'] = b,
    ['r'] = r,
    ['q'] = q,
    ['k'] = k
};

U64 bitboards[12];

U64 occupancies[3];

int side;

int enpassant = no_sq;

int castle;

//Bitboard of 1 except 0 for the a file (pawns)
const U64 not_a_file = 18374403900871474942ULL;

//Bitboard of 1 except 0 for the h file (pawns)
const U64 not_h_file = 9187201950435737471ULL;

//Bitboard of 1 except 0 for the h and g files (knights)
const U64 not_hg_file = 4557430888798830399ULL;

//Bitboard of 1 except 0 for the a and b files (knights)
const U64 not_ab_file = 18229723555195321596ULL;