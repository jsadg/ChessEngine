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
    parse_fen("4k3/Q7/4K3/8/8/8/8/8 w - - ");
    print_board();
    search_position(6);
    
    return 0;

}