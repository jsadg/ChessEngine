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
    printf("%s%s%c", square_to_coords[get_source(move)], square_to_coords[get_target(move)], promoted_pieces[get_promoted(move)]);
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


U64 perft(int depth){
    moves move_list[256];
    U64 nodes = 0;
    if(depth == 0){
        return 1ULL;
    }
    generate_moves(move_list);
    // Iterate through all moves
    for(int count = 0; count < move_list->count; count++){
        int move = move_list->moves[count];
        // If not legal disregard
        if(!make_move(move)){
            continue;
        }
        // Otherwise add to nodes
        nodes += perft(depth -1);
        take_back();
    }
    return nodes;
}

void perft_divide(int depth) {
    long long current_time = current_time_in_ms();
    moves move_list[1];
    generate_moves(move_list);
    U64 nodes, total = 0;

    for (int i = 0; i < move_list->count; i++) {
        int move = move_list->moves[i];
        if (!make_move(move))
            continue;
        nodes = perft(depth - 1);
        take_back();
        print_move(move);
        printf("\n%llu\n", nodes);
        total += nodes;
    }
    printf("Depth: %d\n", depth);
    printf("Total Nodes: %llu\n", total);
    printf("Time taken: %llu ms\n", current_time_in_ms() - current_time);   
}

int parse_move(char * move_string){
    moves move_list[1];
    generate_moves(move_list);

    // Get source and target from the move
    int source_square = (move_string[0] - 'a') + (8 - (move_string[1] - '0')) * 8;
    int target_square = (move_string[2] - 'a') + (8 - (move_string[3] - '0')) * 8;

    // Find move in move list
    for(int move_count = 0; move_count < move_list->count; move_count++){
        int move = move_list->moves[move_count];
        int promoted_piece = get_promoted(move);
        if(source_square == get_source(move) && target_square == get_target(move)){
            if(promoted_piece){
                // Check which promoted piece
                if(move_string[4] == 'q' && (promoted_piece == Q || promoted_piece == q)){
                    return move;
                }
                else if(move_string[4] == 'r' && (promoted_piece == R || promoted_piece == r)){
                    return move;
                }
                else if(move_string[4] == 'b' && (promoted_piece == B || promoted_piece == b)){
                    return move;
                }
                else if(move_string[4] == 'n' && (promoted_piece == N || promoted_piece == n)){
                    return move;
                }
                // Check other promotions
                continue;
            }  
            // Legal move
            return move;
        }
    }
    // Illegal move
    return 0;
}

void parse_position(char *command){
    // Next token location (after position command)
    command += 9;
    char *current_char = command;

    // Parse UCI "startpos"
    if(strncmp(command, "startpos", 8) == 0){
        parse_fen(starting_position);
    }
    // Parse "fen" command
    else{
        current_char = strstr(command, "fen");
        // If null fen then startpos
        if(current_char == NULL){
            parse_fen(starting_position);
        }
        else{
            // Shift pointer to fen string
            current_char += 4;
            parse_fen(current_char);
        }
    }
    // Parse moves after given position
    current_char = strstr(command, "moves");
    if(current_char != NULL){
        // Shift pointer to start of moves
        current_char += 6;
        while(*current_char){
            // Parse next move
            int move = parse_move(current_char);
            // If illegal move break
            if(move == 0){
                break;
            }
            make_move(move);

            // Move on to the next move
            while(*current_char && *current_char != ' '){
                current_char++;
            }
            current_char++;
        }
    }
}

void parse_go(char *command){
    int depth = -1;
    char *current_depth = NULL;
    if((current_depth = strstr(command, "depth"))){
        // Get depth value from command
        depth = atoi(current_depth + 6);
    }
    printf("Depth: %d\n",depth);

    search_position(depth);
}

void uci_loop(){
    // Reset STDIN and STDOUT buffers
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    
    // GUI input buffer
    char input[2000];

    // Engine info
    printf("id name engine\n");
    printf("id author jsadg\n");
    printf("uciok\n");

    // Main loop
    while(1){
        // Reset GUI input
        memset(input, 0, sizeof(input));
        // Flush output
        fflush(stdout);
        // Get GUI input
        if(!fgets(input, 2000, stdin)){
            continue;
        }
        // Make sure input is available
        else if(input[0] == '\n'){
            continue;
        }

        // Parse UCI "isready" command
        else if(strncmp(input, "isready", 7) == 0){
            printf("readyok\n");
            continue;
        }
        // Parse UCI "position" command
        else if(strncmp(input, "position", 8) == 0){
            parse_position(input);
        }
        // Parse UCI "ucinewgame" command
        else if(strncmp(input, "ucinewgame", 11) == 0){
            parse_position("position startpos");
        }
        // Parse UCI "go" command
        else if(strncmp(input, "go", 2) == 0){
            parse_go(input);
        }
        // Parse UCI "quit" command
        else if(strncmp(input, "quit", 4) == 0){
            break;
        }
        // Parse UCI "uci" command
        else if(strncmp(input, "uci", 3) == 0){
             // Engine info
            printf("id name engine\n");
            printf("id author jsadg\n");
            printf("uciok\n");
        }
    }
}