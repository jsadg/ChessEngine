#ifndef IO_H
#define IO_H

#include <stdio.h>
#include "bitboard.h"
#include "board.h"
#include "movegen.h"


// Print the given bitboard out in row column format
void print_bitboard(U64 bitboard);

// Print the current board in row column format and board variables
void print_board(void);

// Convert a given FEN to board position
void parse_fen(char *fen);

// Print all squares attacked by a given side
void print_attacked_squares(int side);

// Print a single move in coordinate notation with relevant flags
void print_move(int move);

// Print all moves in a move list
void print_move_list(moves *move_list);

#endif
