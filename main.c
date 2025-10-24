#include "bitboard.h"
#include "board.h"
#include "attacks.h"
#include "io.h"
#include "movegen.h"


int main(){
    init_piece_attack_tables();

    parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KkQq - ");
    print_board();
    long long current_time = current_time_in_ms();
    U64 test = perft(6);
    printf("Nodes: %llu\n", test);
    printf("Time taken: %llu ms\n", current_time_in_ms() - current_time);
}