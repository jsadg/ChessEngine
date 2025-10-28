#include "bitboard.h"
#include "board.h"
#include "attacks.h"
#include "io.h"
#include "movegen.h"


int main(){
    init_piece_attack_tables();

    uci_loop();

    return 0;

}