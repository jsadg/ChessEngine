#ifndef BOARD_H
#define BOARD_H

#include "bitboard.h"

// Board squares a8=0, h1=63, no_sq used for enpassant calculations
enum{
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1, no_sq
};

// Board colors enum
enum{
    white, black, both
};

// Piece enum, capital = white, lowercase = black
enum {P, N, B, R, Q, K, p, n, b, r, q, k};

// Indexable array of square names e.g. "a8"
extern const char *square_to_coords[];

// Indexable array of piece types e.g. P
extern char ascii_pieces[12];

// Fen for the starting position of chess
#define starting_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KkQq - "

// Matches ascii piece to piece e.g. "P" to P
extern int char_pieces[128];

// Piece location bitboards (one for each piece type)
extern U64 bitboards[12];

// Color occupancy bitboards (white, black, both)
extern U64 occupancies[3];

// Side variable (white or black)
extern int side;

// Enpassant square
extern int enpassant;

// Castle variable
extern int castle;

/*
    Castling representation
    0001 white can castle kingside
    0010 white can castle queenside
    0100 black can castle kingside
    1000 black can castle queenside
*/
//wkc = white kingsde castle
//wqc = white queenside castle
//bkc = black kingsde castle
//wqc = black queenside castle

enum {wkc=1, wqc=2, bkc=4, bqc=8};

// Bitboard of 1 except 0 for the a file (used for pawns)
extern const U64 not_a_file;

// Bitboard of 1 except 0 for the h file (used for pawns)
extern const U64 not_h_file;

// Bitboard of 1 except 0 for the h and g files (used for knights)
extern const U64 not_hg_file;

// Bitboard of 1 except 0 for the a and b files (used for knights)
extern const U64 not_ab_file;

#endif
