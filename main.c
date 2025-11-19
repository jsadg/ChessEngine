#include "bitboard.h"
#include "board.h"
#include "attacks.h"
#include "io.h"
#include "movegen.h"
#include "search.h"

//"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KkQq - "

int main(){
    init_piece_attack_tables();
    init_middlegame_psqt(); 
    init_endgame_psqt();

    uci_loop();

    return 0;
}