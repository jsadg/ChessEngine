#include "eval.h"

static const int material_score[12] = {
    100,    // White pawn
    325,    // White knight
    340,    // White bishop
    500,    // White rook
    950,    // White queen
    10000,  // White king
    -100,   //Black pawn
    -325,   // Black knight
    -350,   // Black bishop
    -500,   // Black rook
    -950,   // Black queen
    -10000  // Black king
};

// Space/center gaining moves good, promotion good, king safety good, overextend bad
static const int pawn_score[64] = {
    70,  70,  70,  70,  70,  70,  70,  70,
    30,  30,  30,  30,  30,  30,  30,  30,
    20,  20,  20,  35,  35,  20,  20,  20,
    17,  15,  15,  40,  40,  15,  15,  17,
    15,   0,  20,  40,  42,   5,   0,  15,
    10,   0,  15,  25,  25,  -5,   5,  10,
     5,   5,   5, -15, -15,   5,   5,   5,
     0,   0,   0,   0,   0,   0,   0,   0
};

// Push em' baby, outside pawns better
static const int pawn_endgame_score[64] = {
    50,  50,  50,  50,  50,  50,  50,  50,
    37,  37,  37,  37,  37,  37,  37,  37,
    27,  27,  27,  27,  27,  27,  27,  27,
    17,  17,  17,  17,  17,  17,  17,  17,
    10,  10,  10,  10,  10,  10,  10,  10,
     5,   5,   5,   5,   5,   5,   5,   5,
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0
};

// Corners and edge bad, motivation to develop
static const int knight_score[64] = {
   -20, -10, -10, -10, -10, -10, -10, -20,
   -10,  -5,   0,   0,   0,   0,  -5, -10,
   -10,   0,  13,  15,  15,  13,   0, -10,
   -10,   0,  13,  17,  17,  13,   0, -10,
   -10,   0,  13,  17,  17,  13,   0, -10,
   -10,   0,  15,   5,   5,  20,   0, -10,
   -10,  -5,   0,  10,  10,   0,  -5, -10,
   -20, -10, -10, -10, -10, -10, -10, -20
};
// Corners bad, own side good, motivation to develop
static const int bishop_score[64] = {
   -10,   0,   0,   0,   0,   0,   0,  -10,
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,
     0,  18,   0,   0,   0,   0,  18,   0,
    12,   0,  20,   0,   0,  20,   0,  12,
     0,  10,   0,  15,  15,   0,  10,   0,
     5,  20,   0,  15,  15,   0,  20,   5,
   -10,   0,  -8,   0,   0,  -8,   0,  -10
};

// Central files good, 7th and 8th ranks good
static const int rook_score[64] = {
    25,  25,  25,  25,  25,  25,  25,  25,
    35,  35,  35,  35,  35,  35,  35,  35,
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,
    -5,   5,  10,  25,  25,  10,   5,   0
};

// Home area ok, center ok, main diagonal ok
static const int queen_score[64] = {
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   5,   5,   5,   5,   0,   0,
    10,   0,   5,  10,  10,   5,   0,  10,
     0,  15,   0,  10,  10,   0,   0,   0,
     0,   0,  15,  15,  15,   0,   0,   0,
     0,   0,   0,  10,   0,   0,   0,   0
};

// Centralized is best
static const int queen_endgame_score[64] = {
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   5,   5,   5,   5,   5,   5,   0,
     0,   5,  15,  15,  15,  15,   5,   0,
     0,   5,  15,  30,  30,  15,   5,   0,
     0,   5,  15,  30,  30,  15,   5,   0,
     0,   5,  15,  15,  15,  15,   5,   0,
     0,   5,   5,   5,   5,   5,   5,   0,
     0,   0,   0,   0,   0,   0,   0,   0,
};

// Everywhere except castled bad
static const int king_score[64] = {
   -20, -20, -20, -20, -20, -20, -20, -20,
   -20, -20, -20, -20, -20, -20, -20, -20,
   -20, -20, -20, -20, -20, -20, -20, -20,
   -20, -20, -20, -20, -20, -20, -20, -20,
   -20, -20, -20, -20, -20, -20, -20, -20,
   -20, -20, -20, -20, -20, -20, -20, -20,
     0,   0, -15, -15, -15, -15,   0,   0,
     5,  30,  25, -10, -10,  -5,  40,   5 
};

// Active king
static const int king_endgame_score[64] = {
     0,   0,   0,   0,   0,   0,   0,   0,
    10,  20,  20,  20,  20,  20,  20,  10,
    15,  30,  40,  40,  40,  40,  30,  15,
    25,  35,  40,  40,  40,  40,  35,  25,
    15,  30,  30,  30,  30,  30,  30,  15,
     0,  20,  20,  20,  20,  20,  20,   0,
    -5,  10,  10,  10,  10,  10,  10,  -5,
   -15, -15, -15, -15, -15, -15, -15, -15,
};

static const U64 FILE_MASKS[8] = {
    0x0101010101010101ULL, // A file
    0x0202020202020202ULL, // B file
    0x0404040404040404ULL, // C file
    0x0808080808080808ULL, // D file
    0x1010101010101010ULL, // E file
    0x2020202020202020ULL, // F file
    0x4040404040404040ULL, // G file
    0x8080808080808080ULL  // H file
};


int middlegame_piece_square_total[12][64];
int endgame_piece_square_total[12][64];


// Used to flip for black
static inline int mirror_square(int square){
    return square ^ 56;
}

void init_middlegame_psqt(){
    for(int piece = P; piece <= k; piece++){
        int pos_bonus;
        for(int square = 0; square < 64; square++){
            switch(piece){
                case P:
                    pos_bonus = pawn_score[square];
                    break;
                case N:
                    pos_bonus = knight_score[square];
                    break;
                case B:
                    pos_bonus = bishop_score[square];
                    break;
                case R:
                    pos_bonus = rook_score[square];
                    break;
                case Q:
                    pos_bonus = queen_score[square];
                    break;
                case K:
                    pos_bonus = king_score[square];
                    break;
                case p:
                    pos_bonus = -pawn_score[mirror_square(square)];
                    break;
                case n:
                    pos_bonus = -knight_score[mirror_square(square)];
                    break;
                case b:
                    pos_bonus = -bishop_score[mirror_square(square)];
                    break;
                case r:
                    pos_bonus = -rook_score[mirror_square(square)];
                    break;
                case q:
                    pos_bonus = -queen_score[mirror_square(square)];
                    break;
                case k:
                    pos_bonus = -king_score[mirror_square(square)];
                    break;
            }
            middlegame_piece_square_total[piece][square] = material_score[piece] + pos_bonus;
        }
    }
}

void init_endgame_psqt(){
    for(int piece = P; piece <= k; piece++){
        int pos_bonus;
        for(int square = 0; square < 64; square++){
            switch(piece){
                case P:
                    pos_bonus = pawn_endgame_score[square];
                    break;
                case N:
                    pos_bonus = knight_score[square];
                    break;
                case B:
                    pos_bonus = bishop_score[square];
                    break;
                case R:
                    pos_bonus = rook_score[square];
                    break;
                case Q:
                    pos_bonus = queen_endgame_score[square];
                    break;
                case K:
                    pos_bonus = king_endgame_score[square];
                    break;
                case p:
                    pos_bonus = -pawn_endgame_score[mirror_square(square)];
                    break;
                case n:
                    pos_bonus = -knight_score[mirror_square(square)];
                    break;
                case b:
                    pos_bonus = -bishop_score[mirror_square(square)];
                    break;
                case r:
                    pos_bonus = -rook_score[mirror_square(square)];
                    break;
                case q:
                    pos_bonus = -queen_endgame_score[mirror_square(square)];
                    break;
                case k:
                    pos_bonus = -king_endgame_score[mirror_square(square)];
                    break;
            }
            endgame_piece_square_total[piece][square] = material_score[piece] + pos_bonus;
        }
    }
}

int calc_pawn_structure(){
    // Get file of all white pawns
    int white_pawn_array[8] = {0};
    U64 bitboard = bitboards[P];
    int square = 0;
    int file = 0;
    U64 left_file = 0ULL;
    U64 right_file = 0ULL;
    int white_isolated = 0;

    while(bitboard){
        square = get_ls1b_index(bitboard);
        white_pawn_array[square % 8]++;

        // Isolated calculation
        file = square % 8;
        left_file = (file > 0) ? FILE_MASKS[file - 1] : 0;
        right_file = (file < 7) ? FILE_MASKS[file + 1] : 0;
        if ((bitboards[P] & (left_file | right_file)) == 0){
            white_isolated++;
        }

        rem_bit(bitboard, square);
    }

    int white_doubled = 0;
    // Calculate number of doubled pawns
    for(int i = 0; i < 8; i++){
        if(white_pawn_array[i] >= 2){
            white_doubled += (white_pawn_array[i]-1);
        }
    }
    int white_islands = 0;
    // Calculate pawn islands
    for(int i = 0; i < 8; i++){
        if(white_pawn_array[i] != 0){
            white_islands++;
            while(i < 8 && white_pawn_array[i] != 0){
                i++;
            }
        }
    }

    int black_pawn_array[8] = {0};
    bitboard = bitboards[p];

    // Calculate isolated pawns
    int black_isolated = 0;

    while(bitboard){
        square = get_ls1b_index(bitboard);
        black_pawn_array[square % 8]++;

        // Isolated calculation
        file = square % 8;
        left_file = (file > 0) ? FILE_MASKS[file - 1] : 0;
        right_file = (file < 7) ? FILE_MASKS[file + 1] : 0;
        if ((bitboards[p] & (left_file | right_file)) == 0){
            black_isolated++;
        }
        rem_bit(bitboard, square);
    }
    int black_doubled = 0;
    // Calculate number of doubled pawns
    for(int i = 0; i < 8; i++){
        if(black_pawn_array[i] >= 2){
            black_doubled += (black_pawn_array[i]-1);
        }
    }
    int black_islands = 0;
    // Calculate pawn islands
    for(int i = 0; i < 8; i++){
        if(black_pawn_array[i] != 0){
            black_islands++;
            while(i < 8 && black_pawn_array[i] != 0){
                i++;
            }
        }
    }

    int score = 0;

    // Reverse of score since these are penalties    
    score -= (white_doubled*15) + (white_islands * white_islands * 2) + (white_isolated * 20);
    score += (black_doubled*15) + (black_islands * black_islands * 2) + (black_isolated * 20);
    return score;
}


int calc_piece_mobility(){
    int source_square;
    U64 bitboard;
    U64 attacks;
    int mobility = 0;
    for(int piece = P; piece <= k; piece++){
        // Get a copy of the piece bitboard
        bitboard = bitboards[piece];
        // Pawns and king moves should not be counted for
        if(piece == P || piece == p || piece == K || piece == k){
            continue;
        }
        // White knight move counting
        if(piece == N){
            while(bitboard){
                source_square = get_ls1b_index(bitboard);
                // Get knight attacks
                attacks = knight_attacks[source_square] & ~occupancies[white];
                mobility += count_bits(attacks)*8;
                rem_bit(bitboard, source_square);
            }
        }
        // Black knight move counting
        if(piece == n){
            while(bitboard){
                source_square = get_ls1b_index(bitboard);
                attacks = knight_attacks[source_square] & ~occupancies[black];
                mobility -= count_bits(attacks)*8;
                rem_bit(bitboard, source_square);
            }
        }

        // White bishop move generation
        if(piece == B){
            while(bitboard){
                source_square = get_ls1b_index(bitboard);
                // Get bishop attacks
                attacks = get_bishop_attacks(source_square, occupancies[both]) & ~occupancies[white];
                // Increased value for bishop mobiliity
                mobility += count_bits(attacks)*11;
                rem_bit(bitboard, source_square);
            }
        }
        // Black bishop move counting
        if(piece == b){
            while(bitboard){
                source_square = get_ls1b_index(bitboard);
                // Get bishop attacks
                attacks = get_bishop_attacks(source_square, occupancies[both]) & ~occupancies[black];
                // Increased value for bishop mobiliity
                mobility -= count_bits(attacks)*11;
                rem_bit(bitboard, source_square);
            }
        }

        // White rook move counting
        if(piece == R){
            while(bitboard){
                source_square = get_ls1b_index(bitboard);
                // Get rook attacks
                attacks = get_rook_attacks(source_square, occupancies[both]) & ~occupancies[white];
                // Less value for rook mobility
                mobility += count_bits(attacks)*5;
                rem_bit(bitboard, source_square);
            }
        }
        // Black rook move counting
        if(piece == r){
            while(bitboard){
                source_square = get_ls1b_index(bitboard);
                // Get rook attacks
                attacks = get_rook_attacks(source_square, occupancies[both]) & ~occupancies[black];
                // Less value for rook mobility
                mobility -= count_bits(attacks)*5;
                rem_bit(bitboard, source_square);
            }
        }
        // White queen move counting
        if(piece == Q){
            while(bitboard){
                source_square = get_ls1b_index(bitboard);
                // Get queen attacks
                attacks = get_queen_attacks(source_square, occupancies[both]) & ~occupancies[white];
                // Even less value for queen mobility
                mobility += count_bits(attacks)*2;
                rem_bit(bitboard, source_square);
            }
        }
        // Black queen move counting
        if(piece == q){
            while(bitboard){
                source_square = get_ls1b_index(bitboard);
                // Get queen attacks
                attacks = get_queen_attacks(source_square, occupancies[both]) & ~occupancies[black];
                // Even less value for queen mobility
                mobility -= count_bits(attacks)*2;
                rem_bit(bitboard, source_square);
            }
        }
    }
    return mobility/10;
}

int calc_piece_bonuses(){
    int score = 0;

    // Bishop pair calculations
    int num_white_bishops = count_bits(bitboards[B]);
    int num_black_bishops = count_bits(bitboards[b]);
    if(num_white_bishops >= 2){
        score += 60;
    }
    if(num_black_bishops >= 2){
        score -= 60;
    }

    // Open/half open files for rook bonus
    int square = 0;
    int file = 0;
    U64 in_front = 0ULL;
    int friendly_pawn = 0;
    int enemy_pawn = 0;
    U64 white_rooks = bitboards[R];
    U64 black_rooks = bitboards[r];

    // Find type of pawns in front
    while(white_rooks){
        square = get_ls1b_index(white_rooks);
        file = square % 8;

        //                   File Mask                 Squares in front mask
        in_front = ((0x0101010101010101ULL << file) & (~0ULL << (square + 1)));

        friendly_pawn = bitboards[P] & in_front;
        enemy_pawn = bitboards[p] & in_front;

        // In middle game prioritize open files
        if(get_game_state() == 0){
            // Closed file
            if(friendly_pawn){
                score -= 5;
            }
            // Half open file
            else if(enemy_pawn){
                score += 5;
            }
            // Open file
            else{
                score += 15;
            }
        }
        // In endgame put rook behind passed pawns
        else if(get_game_state() == 1){
            // Closed file
            if(friendly_pawn && enemy_pawn){
                score -= 5;
            }
            // Behind passed pawn
            else if(friendly_pawn){
                score += 25;
            }
            // Attacking enemy pawn or open file
            else{
                score += 5;
            }
        }

        rem_bit(white_rooks, square);
    }

    while(black_rooks){
        square = get_ls1b_index(black_rooks);
        file = square % 8;
        //                   File Mask                 Squares in front mask
        in_front = ((0x0101010101010101ULL << file) & ((1ULL << square) - 1));
                                                  // From 000010000 to 000001111 etc
        friendly_pawn = bitboards[p] & in_front;
        enemy_pawn = bitboards[P] & in_front;

        // In middle game prioritize open files
        if(get_game_state() == 0){
            // Closed file
            if(friendly_pawn){
                score += 5;
            }
            // Half open file
            else if(enemy_pawn){
                score -= 5;
            }
            // Open file
            else{
                score -= 15;
            }
        }
        // In endgame put rook behind passed pawns
        else if(get_game_state() == 1){
            // Closed file
            if(friendly_pawn && enemy_pawn){
                score += 5;
            }
            // Behind passed pawn
            else if(friendly_pawn){
                score -= 25;
            }
            // Attacking enemy pawn or open file
            else{
                score -= 5;
            }
        }

        rem_bit(black_rooks, square);
    }
    return score;
}

// Arrays containing important pawn shield squares and associated penalty
static const int wks_squares[] = {f2, g2, h2};
static const int wks_penalties[] = {12, 30, 12};

static const int wqs_squares[] = {a2, b2, c2};
static const int wqs_penalties[] = {12, 30, 22};

static const int bks_squares[] = {f7, g7, h7};
static const int bks_penalties[] = {12, 30, 12};

static const int bqs_squares[] = {a7, b7, c7};
static const int bqs_penalties[] = {12, 30, 22};



int calc_king_safety(){
    // If endgame position king safety bonus not needed
    if(get_game_state() == 1){
        return 0;
    }
    int score = 0;

    // If white castled kingside:
    if(bitboards[K] & ((1ULL << g1) | (1ULL << h1))){
        // Loop through important king safety squares
        for(int i = 0; i < 3; i++){
            if(bitboards[P] & ((1ULL << wks_squares[i]) | (1ULL << (wks_squares[i] - 8)))){
                score += 6;
            }
            else{
                score -= wks_penalties[i];
            }
        }
    }

    // If white castled queenside:
    if(bitboards[K] & ((1ULL << c1) | (1ULL << b1))){
        // Loop through important king safety squares
        for(int i = 0; i < 3; i++){
            if(bitboards[P] & ((1ULL << wqs_squares[i]) | (1ULL << (wqs_squares[i] - 8)))){
                score += 6;
            }
            else{
                score -= wqs_penalties[i];
            }
        }
    }

    // If black castled kingside:
    if(bitboards[k] & ((1ULL << g8) | (1ULL << h8))){
        // Loop through important king safety squares
        for(int i = 0; i < 3; i++){
            if(bitboards[p] & ((1ULL << bks_squares[i]) | (1ULL << (bks_squares[i] + 8)))){
                score -= 6;
            }
            else{
                score += bks_penalties[i];
            }
        }
    }

    // If black castled queenside:
    if(bitboards[k] & ((1ULL << c8) | (1ULL << b8))){
        // Loop through important king safety squares
        for(int i = 0; i < 3; i++){
            if(bitboards[p] & ((1ULL << bqs_squares[i]) | (1ULL << (bqs_squares[i] + 8)))){
                score -= 6;
            }
            else{
                score += bqs_penalties[i];
            }
        }
    }
    return score;
}

int evaluate(){
    int score = 0;
    U64 bitboard;
    int square;
    // Loop through pieces and get eval from each
    for(int piece = P; piece <= k; piece++){
        bitboard = bitboards[piece];
        while(bitboard){
            square = get_ls1b_index(bitboard);
            // Piece weights based on game state
            if(get_game_state() == 0){
                score += middlegame_piece_square_total[piece][square];
            }
            else{
                score += endgame_piece_square_total[piece][square];
            }
            rem_bit(bitboard, square);
        }
    }
    // Add pawn structure penalties
    score += calc_pawn_structure();

    // Add piece mobility bonuses
    score += calc_piece_mobility();
    
    // Add bishop pair bonus
    score += calc_piece_bonuses();
    
    score += calc_king_safety();

    return (side == white) ? score : -score;
}
