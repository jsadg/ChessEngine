#include "bitboard.h"
#include "board.h"
#include "attacks.h"
#include "io.h"
#include "movegen.h"
#include "search.h"

//"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KkQq - "

int main(){
    init_piece_attack_tables();
    init_psqt(); 

    uci_loop();
    /*
    parse_fen(starting_position);
    print_board();
    int d = evaluate();
    printf("Eval: %d\n", d);
    search_position(5);
    return 0; */
}