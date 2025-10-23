#include "bitboard.h"
#include "board.h"
#include "movegen.h"
#include "io.h"

void print_bitboard(U64 bitboard){
    for(int rank = 0; rank < 8; rank++){
        for(int file = 0; file < 8; file++){
            int square = rank * 8 + file;
            if(file == 0){
                printf(" %d ", 8 - rank);
            }
            printf(" %d", get_bit(bitboard, square) ? 1 : 0);
        }
        printf("\n");
    }
    printf("    a b c d e f g h\n\n");

    // Decimal representation of bitboard
    printf("Bitboard: %llud\n\n", bitboard);
}

void print_board(){
    for(int rank = 0; rank < 8;rank++){
        for(int file = 0; file < 8; file++){
            int square = rank * 8 + file;
            if(!file){
                printf(" %d ", 8 - rank);
            }
            int piece=-1;
            for(int bb_piece=P; bb_piece <= k; bb_piece++){
                if(get_bit(bitboards[bb_piece], square)){
                    piece = bb_piece;
                }
            }
            printf(" %c", (piece == -1) ? '.' : ascii_pieces[piece]);
        }
        printf("\n");
    }
    // Board files
    printf("    a b c d e f g h\n\n");
    // Side to move
    printf("Side:  %s\n", !side ? "white" : "black");
    // Enpasssant square
    printf("Enpassant:  %s\n", (enpassant != no_sq) ? square_to_coords[enpassant]: "no");
    // Castling rights
    printf("Castling: %c%c%c%c\n\n", (castle & wkc) ? 'K' : '-', (castle & wqc) ? 'Q' : '-', (castle & bkc) ? 'k' : '-', (castle & bqc) ? 'q' : '-');
}

void parse_fen(char* fen){
    // Reset board and occupancies
    memset(bitboards, 0ULL, sizeof(bitboards));
    memset(occupancies, 0ULL, sizeof(occupancies));
    // Reset variables
    side = 0;
    enpassant = no_sq;
    castle = 0;
    for(int rank = 0; rank < 8; rank ++){
        for(int file = 0; file < 8; file++){
            int square = rank * 8 + file;
            // Match piece from FEN
            if((*fen >= 'a' && *fen <= 'z') || (*fen >= 'A' && *fen <= 'Z')){
                int piece = char_pieces[(unsigned char)*fen];
                set_bit(bitboards[piece], square);
                fen++;
            }
            // Match empty space from FEN
            if(*fen >= '0' && *fen <= '9'){
                // Convert char 0 to int 0
                int offset = *fen - '0';
                int piece=-1;
                for(int bb_piece=P; bb_piece <= k; bb_piece++){
                    if(get_bit(bitboards[bb_piece], square)){
                        piece = bb_piece;
                    }
                }
                // If no piece on square
                if(piece == -1){
                    file--;
                }
                // Adjust file counter
                file += offset;
                fen++;
            }
            if(*fen == '/'){
                fen++;
            }

        }
    }
    // Move to side to move
    fen++;
    (*fen == 'w') ? (side = white) : (side = black);
    // Move to castling rights
    fen+=2;
    while(*fen != ' '){
        switch(*fen){
            case 'K': castle |= wkc; break;
            case 'Q': castle |= wqc; break;
            case 'k': castle |= bkc; break;
            case 'q': castle |= bqc; break;
            case '-': break;
        }
        fen++;
    }
    // Move to enpassant square
    fen++;
    if(*fen != '-'){
        int file = fen[0] - 'a';
        int rank = 8 - (fen[1] - '0');
        enpassant = rank * 8 + file;
    }
    else{
        enpassant = no_sq;
    }
    // Loop over white piece bitboards
    for(int piece = P; piece <= K; piece++){
        occupancies[white] |= bitboards[piece];
    }
    // Loop over black piece bitboards
    for(int piece = p; piece <= k; piece++){
        occupancies[black] |= bitboards[piece];
    }
    occupancies[both] = occupancies[white] | occupancies[black];
}

void print_attacked_squares(int side){
    for(int rank = 0; rank < 8; rank++){
        for(int file = 0; file < 8; file++){
            int square = rank * 8 + file;
            if(!file){
                printf("  %d ", 8-rank);
                //Checks if square is attacked
            }
            printf("%d ", is_square_attacked(square, side) ? 1 : 0);
        }
        printf("\n");
    }
    printf("    a b c d e f g h\n\n");
}

void print_move(int move){
    printf("%s%s%c\n", square_to_coords[get_source(move)], square_to_coords[get_target(move)], promoted_pieces[get_promoted(move)]);
}

void print_move_list(moves *move_list){
    if(!move_list->count){
        printf("No moves in move list\n");
        return;
    }
    printf("\nmove   piece    capture    double    enpassant   castling\n");
    for(int move_count = 0; move_count < move_list->count; move_count++){
        int move = move_list->moves[move_count];
        // Move flags
        printf("%s%s%c   %c        %d          %d         %d           %d\n",
            square_to_coords[get_source(move)],
            square_to_coords[get_target(move)],
            promoted_pieces[get_promoted(move)],
            ascii_pieces[get_piece(move)],
            get_capture(move),
            get_double(move),
            get_enpassant(move),
            get_castling(move));
    }
    printf("\n\n Total number of moves: %d\n\n", move_list->count);
}