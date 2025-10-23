#include "bitboard.h"
#include "board.h"
#include "attacks.h"
#include "io.h"
#include "movegen.h"


int main(){
    init_piece_attack_tables();

    parse_fen("1k6/2RQ3N/1n2NP2/2pp2pn/q7/4r3/5Pb1/1K6 w - - 0 1");
    print_board();
    
    moves move_list[1];

    generate_moves(move_list);

    for(int count = 0; count < move_list->count; count++){
        int move = move_list->moves[count];

        copy_board();

        if(!make_move(move, all_moves)){
            continue;
        }
        
        print_board();
        getchar();
        take_back();
        print_board();
        getchar();
    }


}