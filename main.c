#include "bitboard.h"
#include "board.h"
#include "attacks.h"
#include "io.h"
#include "movegen.h"


int main(){
    init_piece_attack_tables();

    parse_fen("rnbqkbnr/Pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KkQq - ");
    print_board();
    int move = parse_move("a7b8n");
    printf("Move: %d\n", move);
    if(move){
        make_move(move, all_moves);
    }
    print_board();

}