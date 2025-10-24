#include "bitboard.h"
#include "board.h"
#include "magics.h"

U64 pawn_attacks[2][64];

U64 mask_pawn_attacks(int side, int square){
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;
    set_bit(bitboard, square);

    if(!side){
        // White pawns cannot attack left on a-file and right and h-file
        if((bitboard >> 7) & not_a_file){
            attacks |= (bitboard >> 7);
        }
        if((bitboard >> 9) & not_h_file){
            attacks |= (bitboard >> 9);
        }
    }
    else{
        // Black pawns cannot attack left on h-file and right and a-file
        if((bitboard << 7) & not_h_file){
            attacks |= (bitboard << 7);
        }
        if((bitboard << 9) & not_a_file){
            attacks |= (bitboard << 9);
        }
    }
    return attacks;
}

void init_pawn_attack_table(){
    for(int square = 0; square < 64; square++){
        pawn_attacks[white][square] = mask_pawn_attacks(white, square);
        pawn_attacks[black][square] = mask_pawn_attacks(black, square);
    }
}

U64 knight_attacks[64];

U64 mask_knight_attacks(int square){
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;
    set_bit(bitboard, square);

    // Prevent board leakovers with knight moves
    if((bitboard >> 17) & not_h_file){
        attacks |= (bitboard >> 17);
    }
    if((bitboard >> 15) & not_a_file){
        attacks |= (bitboard >> 15);
    }
    if((bitboard >> 10) & not_hg_file){
        attacks |= (bitboard >> 10);
    }
    if((bitboard >> 6) & not_ab_file){
        attacks |= (bitboard >> 6);
    }
    if((bitboard << 17) & not_a_file){
        attacks |= (bitboard << 17);
    }
    if((bitboard << 15) & not_h_file){
        attacks |= (bitboard << 15);
    }
    if((bitboard << 10) & not_ab_file){
        attacks |= (bitboard << 10);
    }
    if((bitboard << 6) & not_hg_file){
        attacks |= (bitboard << 6);
    }
    return attacks;
}

void init_knight_attack_table(){
    for(int square=0; square<64; square++){
        knight_attacks[square] = mask_knight_attacks(square);
    }
}

U64 king_attacks[64];

U64 mask_king_attacks(int square){
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;
    set_bit(bitboard, square);

    // Prevent board leakovers
    // Move diagonally up right 1
    if((bitboard >> 7) & not_a_file){
        attacks |= (bitboard >> 7);
    }
    // Move up one
    if((bitboard >> 8)){
        attacks |= (bitboard >> 8);
    }
    // Move diagonally up left 1
    if((bitboard >> 9) & not_h_file){
        attacks |= (bitboard >> 9);
    }
    // Move diagonally down left 1
    if((bitboard << 7) & not_h_file){
        attacks |= (bitboard << 7);
    }
    // Move down 1
    if((bitboard << 8)){
        attacks |= (bitboard << 8);
    }
    // Move diagonally down right 1
    if((bitboard << 9) & not_a_file){
        attacks |= (bitboard << 9);
    }
    // Move left 1
    if((bitboard << 1) & not_a_file){
        attacks |= (bitboard << 1);
    }
    // Move right 1
    if((bitboard >> 1) & not_h_file){
        attacks |= (bitboard >> 1);
    }
    return attacks;
}

void init_king_attack_table(){
    for(int square=0; square<64; square++){
        king_attacks[square] = mask_king_attacks(square);
    }
}

U64 set_occupancy(int index, int bits_in_mask, U64 attack_mask){
    U64 occupancy = 0ULL;
    // Cycle through attack_mask and move bits to occupancy
    for(int count = 0; count < bits_in_mask; count++){
        int square = get_ls1b_index(attack_mask);
        rem_bit(attack_mask, square);
        if(index & (1<<count)){
            occupancy |= (1ULL << square);
        }
    }
    return occupancy;
}

U64 bishop_masks[64];
U64 bishop_attacks[64][512];

U64 mask_bishop_attacks(int square){
    U64 attacks = 0ULL;
    int rank, file;
    int trank = square / 8;
    int tfile = square % 8;

    // Bishop mask does not extend to edge squares
    for(rank = trank+1, file = tfile+1; (rank <=6 && file <= 6); rank++, file++){
        attacks |= (1ULL << (rank * 8 + file));
    }
    for(rank = trank-1, file = tfile+1; (rank >=1 && file <= 6); rank--, file++){
        attacks |= (1ULL << (rank * 8 + file));
    }
    for(rank = trank+1, file = tfile-1; (rank <=6 && file >= 1); rank++, file--){
        attacks |= (1ULL << (rank * 8 + file));
    }
    for(rank = trank-1, file = tfile-1; (rank >=1 && file >= 1); rank--, file--){
        attacks |= (1ULL << (rank * 8 + file));
    }
    return attacks;
}

U64 bishop_attacks_game(int square, U64 block){
    U64 attacks = 0ULL;
    int rank, file;
    int trank = square / 8;
    int tfile = square % 8;
    // In-game bishop attacks covers edge squares
    for(rank = trank+1, file = tfile+1; (rank <=7 && file <= 7); rank++, file++){
        attacks |= (1ULL << (rank * 8 + file));
        if(1ULL << (rank * 8 + file) & block){
            break;
        }
    }
    for(rank = trank-1, file = tfile+1; (rank >=0 && file <= 7); rank--, file++){
        attacks |= (1ULL << (rank * 8 + file));
        if(1ULL << (rank * 8 + file) & block){
            break;
        }
    }
    for(rank = trank+1, file = tfile-1; (rank <=7 && file >= 0); rank++, file--){
        attacks |= (1ULL << (rank * 8 + file));
        if(1ULL << (rank * 8 + file) & block){
            break;
        }
    }
    for(rank = trank-1, file = tfile-1; (rank >=0 && file >= 0); rank--, file--){
        attacks |= (1ULL << (rank * 8 + file));
        if(1ULL << (rank * 8 + file) & block){
            break;
        }
    }
    return attacks;
}

void init_bishop_attacks(){
    // Using bishop masks, occupancies, and magic numbers generate bishop attacks
    for(int square=0; square<64; square++){
        bishop_masks[square] = mask_bishop_attacks(square);
        int bit_count = count_bits(bishop_masks[square]);
        int occupancy_indices = (1 << bit_count);
        for(int i=0; i<occupancy_indices; i++){
            U64 occupancy = set_occupancy(i, bit_count, bishop_masks[square]);
            int magic_index = occupancy * bishop_magics[square] >> (64-bishop_bits[square]);
            bishop_attacks[square][magic_index] = bishop_attacks_game(square, occupancy);
        }
    }
}

U64 rook_masks[64];
U64 rook_attacks[64][4096];


U64 mask_rook_attacks(int square){
    U64 attacks = 0ULL;
    int rank, file;
    int trank = square / 8;
    int tfile = square % 8;
    // Rook mask does not extend to edges
    for(rank = trank+1; rank <=6; rank++){
        attacks |= (1ULL << (rank * 8 + tfile));
    }
    for(rank = trank-1; rank >=1; rank--){
        attacks |= (1ULL << (rank * 8 + tfile));
    }
    for(file = tfile+1; file <=6; file++){
        attacks |= (1ULL << (trank * 8 + file));
    }
    for(file = tfile-1; file >=1; file--){
        attacks |= (1ULL << (trank * 8 + file));
    }
    return attacks;
}

U64 rook_attacks_game(int square, U64 block){
    U64 attacks = 0ULL;
    int rank, file;
    int trank = square / 8;
    int tfile = square % 8;
    // Rook attacks in game extends to edeges
    for(rank = trank+1; rank <=7; rank++){
        attacks |= (1ULL << (rank * 8 + tfile));
        if(1ULL << (rank * 8 + tfile) & block){
            break;
        }
    }
    for(rank = trank-1; rank >=0; rank--){
        attacks |= (1ULL << (rank * 8 + tfile));
        if(1ULL << (rank * 8 + tfile) & block){
            break;
        }
    }
    for(file = tfile+1; file <=7; file++){
        attacks |= (1ULL << (trank * 8 + file));
        if(1ULL << (trank * 8 + file) & block){
            break;
        }
    }
    for(file = tfile-1; file >=0; file--){
        attacks |= (1ULL << (trank * 8 + file));
        if(1ULL << (trank * 8 + file) & block){
            break;
        }
    }
    return attacks;
}

void init_rook_attacks(){
    for(int square=0; square<64; square++){
        // Using rook masks, occupancies, and magic numbers generate rook attacks
        rook_masks[square] = mask_rook_attacks(square);
        int bit_count = count_bits(rook_masks[square]);
        int occupancy_indices = (1 << bit_count);
        for(int i=0; i<occupancy_indices; i++){
            U64 occupancy = set_occupancy(i, bit_count, rook_masks[square]);
            int magic_index = occupancy * rook_magics[square] >> (64-rook_bits[square]);
            rook_attacks[square][magic_index] = rook_attacks_game(square, occupancy);
        }
    }
}

void init_piece_attack_tables(){
    init_pawn_attack_table();
    init_knight_attack_table();
    init_king_attack_table();
    init_bishop_attacks();
    init_rook_attacks();
}