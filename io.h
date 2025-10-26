#ifndef IO_H
#define IO_H

#include <stdio.h>
#include "bitboard.h"
#include "board.h"
#include "movegen.h"
#include <sys/time.h>

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

// Returns current time in for perft analysis
static inline long long current_time_in_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

// Performance analysis for movegen
U64 perft(int depth);

// Give each move of the first perft and its perft
void perft_divide(int depth);

// Parse user input
int parse_move(char * move_string);

#endif
