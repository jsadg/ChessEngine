#include <stdio.h>

//Bitboard data type
#define U64 unsigned long long

//Board squares
enum{
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1  
};

enum{
    white, black
};

const char *square_to_coords[] = {
    "a8","b8","c8","d8","e8","f8","g8","h8",
    "a7","b7","c7","d7","e7","f7","g7","h7",
    "a6","b6","c6","d6","e6","f6","g6","h6",
    "a5","b5","c5","d5","e5","f5","g5","h5",
    "a4","b4","c4","d4","e4","f4","g4","h4",
    "a3","b3","c3","d3","e3","f3","g3","h3",
    "a2","b2","c2","d2","e2","f2","g2","h2",
    "a1","b1","c1","d1","e1","f1","g1","h1"
};


//Rook magic numbers sourced from Pradyumna Kannan
static const U64 rook_magics[64] = {
    0x8a80104000800020ULL, 0x140002000100040ULL,   0x2801880a0017001ULL,  0x100081001000420ULL,
    0x200020010080420ULL,   0x3001c0002010008ULL,  0x8480008002000100ULL, 0x2080088004402900ULL,
    0x800098204000ULL,      0x2024401000200040ULL, 0x100802000801000ULL,  0x120800800801000ULL,
    0x208808088000400ULL,   0x2802200800400ULL,    0x2200800100020080ULL, 0x801000060821100ULL,
    0x80044006422000ULL,    0x100808020004000ULL,  0x12108a0010204200ULL, 0x140848010000802ULL,
    0x481828014002800ULL,   0x8094004002004100ULL, 0x4010040010010802ULL, 0x20008806104ULL,
    0x100400080208000ULL,   0x2040002120081000ULL, 0x21200680100081ULL,   0x20100080080080ULL,
    0x2000a00200410ULL,     0x20080800400ULL,      0x80088400100102ULL,   0x80004600042881ULL,
    0x4040008040800020ULL,  0x440003000200801ULL,  0x4200011004500ULL,    0x188020010100100ULL,
    0x14800401802800ULL,    0x2080040080800200ULL, 0x124080204001001ULL,  0x200046502000484ULL,
    0x480400080088020ULL,   0x1000422010034000ULL, 0x30200100110040ULL,   0x100021010009ULL,
    0x2002080100110004ULL,  0x202008004008002ULL,  0x20020004010100ULL,   0x2048440040820001ULL,
    0x101002200408200ULL,   0x40802000401080ULL,   0x4008142004410100ULL, 0x2060820c0120200ULL,
    0x1001004080100ULL,     0x20c020080040080ULL,  0x2935610830022400ULL, 0x44440041009200ULL,
    0x280001040802101ULL,   0x2100190040002085ULL, 0x80c0084100102001ULL, 0x4024081001000421ULL,
    0x20030a0244872ULL,     0x12001008414402ULL,   0x2006104900a0804ULL,  0x1004081002402ULL
};

//Bishop magic numbers sourced from Pradyumna Kannan
static const U64 bishop_magics[64] = {
    0x40040844404084ULL,   0x2004208a004208ULL,   0x10190041080202ULL,  0x108060845042010ULL,
    0x581104180800210ULL,  0x2112080446200010ULL, 0x1080820820060210ULL, 0x3c0808410220200ULL,
    0x4050404440404ULL,    0x21001420088ULL,      0x24d0080801082102ULL, 0x1020a0a020400ULL,
    0x40308200402ULL,      0x4011002100800ULL,    0x401484104104005ULL,  0x801010402020200ULL,
    0x400210c3880100ULL,   0x404022024108200ULL,  0x810018200204102ULL,  0x4002801a02003ULL,
    0x85040820080400ULL,   0x810102c808880400ULL, 0xe900410884800ULL,    0x8002020480840102ULL,
    0x220200865090201ULL,  0x2010100a02021202ULL, 0x152048408022401ULL,  0x20080002081110ULL,
    0x4001001021004000ULL, 0x800040400a011002ULL, 0xe4004081011002ULL,   0x1c004001012080ULL,
    0x8004200962a00220ULL, 0x8422100208500202ULL, 0x2000402200300c08ULL, 0x8646020080080080ULL,
    0x80020a0200100808ULL, 0x2010004880111000ULL, 0x623000a080011400ULL, 0x42008c0340209202ULL,
    0x209188240001000ULL,  0x400408a884001ULL,   0x110400a6080400ULL,   0x1840060a44020800ULL,
    0x90080104000041ULL,   0x201011000808101ULL,  0x1a2208080504f080ULL, 0x8012020600211212ULL,
    0x500861011240000ULL,  0x180806108200800ULL,  0x4000020e01040044ULL, 0x300000261044000aULL,
    0x802241102020002ULL,  0x20906061210001ULL,   0x5a84841004010310ULL, 0x4010801011c04ULL,
    0xa010109502200ULL,    0x4a02012000ULL,       0x500201010098b028ULL, 0x8040002811040900ULL,
    0x28000010020204ULL,   0x6000020202d0240ULL,  0x8918844842082200ULL, 0x4010011029020020ULL
};


//Debruijn board table for lsb calculation
static const int index64[64] = {
     0,  1, 48,  2, 57, 49, 28,  3,
    61, 58, 50, 42, 38, 29, 17,  4,
    62, 55, 59, 36, 53, 51, 43, 22,
    45, 39, 33, 30, 24, 18, 12,  5,
    63, 47, 56, 27, 60, 41, 37, 16,
    54, 35, 52, 21, 44, 32, 23, 11,
    46, 26, 40, 15, 34, 20, 31, 10,
    25, 14, 19,  9, 13,  8,  7,  6
};

//Macros
//Sets a given bit by making the target bit a 1 and oring it
#define set_bit(bitboard, square) ((bitboard) |= (1ULL << (square)))

//Checks a given bit on the board by making the target but a 1 and anding it
#define get_bit(bitboard, square) ((bitboard) & (1ULL << (square)))

//Removes a given bit by making everything a 1 other than the target bit then anding it
#define rem_bit(bitboard, square) ((bitboard) &= ~(1ULL << (square)))

//Counts the bits within the bitboard
static inline int count_bits(U64 bitboard){
    int bitcount = 0;
    bitcount=__builtin_popcountll(bitboard);
    return bitcount;
}

//Gets the lsb location of the bitboard
static inline int get_lsb_index(U64 bitboard) {
    if (bitboard == 0)
        return -1;

    U64 isolated = bitboard & -bitboard;
    U64 debruijn = (isolated * 0x03f79d71b4cb0a89ULL) >> 58;
    return index64[debruijn];
}

//Print the given bitboard
void print_bitboard(U64 bitboard){
    for(int rank = 0; rank < 8; rank++){
        for(int file = 0; file < 8; file++){
            int square = rank*8 + file;
            if(file == 0){
                printf(" %d ", 8 - rank);
            }
            printf(" %d ", get_bit(bitboard, square) ? 1 : 0);
        }
        printf("\n");
    }
    printf("    a  b  c  d  e  f  g  h\n\n");

    //Decimal representation of bitboard
    printf("Bitboard: %llud\n\n", bitboard);
}

//Bitboard of 1 except 0 for the a file (pawns)
const U64 not_a_file = 18374403900871474942ULL;

//Bitboard of 1 except 0 for the h file (pawns)
const U64 not_h_file = 9187201950435737471ULL;

//Bitboard of 1 except 0 for the h and g files (knights)
const U64 not_hg_file = 4557430888798830399ULL;

//Bitboard of 1 except 0 for the a and b files (knights)
const U64 not_ab_file = 18229723555195321596ULL;

//Pawns attack table [side][square]
U64 pawn_attacks[2][64];

//Generate pawn attacks
U64 mask_pawn_attacks(int side, int square){
    //Attacks
    U64 attacks = 0ULL;
    //Piece
    U64 bitboard = 0ULL;
    //Set piece on board
    set_bit(bitboard, square);

    if(!side){
        //Pawns cannot attack left on a file and right and h file
        if((bitboard >> 7) & not_a_file)
            attacks |= (bitboard >> 7);
        if((bitboard >> 9) & not_h_file)
            attacks |= (bitboard >> 9);
    }
    else{
        if((bitboard << 7) & not_h_file)
            attacks |= (bitboard << 7);
        if((bitboard << 9) & not_a_file)
            attacks |= (bitboard << 9);
    }
    return attacks;
}

//Generate the pawn attack lookup table
void init_pawn_attack_table(){
    for(int square=0; square<64; square++){
        pawn_attacks[white][square] = mask_pawn_attacks(white, square);
        pawn_attacks[black][square] = mask_pawn_attacks(black, square);
    }
}

//Knight attacks table [square]
U64 knight_attacks[64];

//Generate knights attacks
U64 mask_knight_attacks(int square){
    //Attacks
    U64 attacks = 0ULL;
    //Piece
    U64 bitboard = 0ULL;
    //Set piece on board
    set_bit(bitboard, square);

    //Prevent board leakovers
    if((bitboard >> 17) & not_h_file)
        attacks |= (bitboard >> 17);
    if((bitboard >> 15) & not_a_file)
        attacks |= (bitboard >> 15);
    if((bitboard >> 10) & not_hg_file)
        attacks |= (bitboard >> 10);
    if((bitboard >> 6) & not_ab_file)
        attacks |= (bitboard >> 6);
    if((bitboard << 17) & not_a_file)
        attacks |= (bitboard << 17);
    if((bitboard << 15) & not_h_file)
        attacks |= (bitboard << 15);
    if((bitboard << 10) & not_ab_file)
        attacks |= (bitboard << 10);
    if((bitboard << 6) & not_hg_file)
        attacks |= (bitboard << 6);
    return attacks;
}

//Generate the knight attack lookup table
void init_knight_attack_table(){
    for(int square=0; square<64; square++){
        knight_attacks[square] = mask_knight_attacks(square);
    }
}

//King attacks table [square]
U64 king_attacks[64];

//Generate king attacks
U64 mask_king_attacks(int square){
    //Attacks
    U64 attacks = 0ULL;
    //Piece
    U64 bitboard = 0ULL;
    //Set piece on board
    set_bit(bitboard, square);

    //Prevent board leakovers
    //Move diagonally up right 1
    if((bitboard >> 7) & not_a_file)
        attacks |= (bitboard >> 7);
    //Move up one
    if((bitboard >> 8))
        attacks |= (bitboard >> 8);
    //Move diagonally up left 1
    if((bitboard >> 9) & not_h_file)
        attacks |= (bitboard >> 9);
    //Move diagonally down left 1
    if((bitboard << 7) & not_h_file)
        attacks |= (bitboard << 7);
    //Move down 1
    if((bitboard << 8))
        attacks |= (bitboard << 8);
    //Move diagonally down right 1
    if((bitboard << 9) & not_a_file)
        attacks |= (bitboard << 9);
    //Move left 1
    if((bitboard << 1) & not_a_file)
        attacks |= (bitboard << 1);
    //Move right 1
    if((bitboard >> 1) & not_h_file)
        attacks |= (bitboard >> 1);
    return attacks;
}

//Generate the king attack lookup table
void init_king_attack_table(){
    for(int square=0; square<64; square++){
        king_attacks[square] = mask_king_attacks(square);
    }
}

U64 set_occupancy(int index, int bits_in_mask, U64 attack_mask){
    U64 occupancy = 0ULL;

    for(int count = 0; count < bits_in_mask; count++){
        int square = get_lsb_index(attack_mask);
        rem_bit(attack_mask, square);
        if(index & (1<<count))
            occupancy |= (1ULL << square);
    }

    return occupancy;
}

//Bishop relevant occupancy bits
const int bishop_bits[64] = {
    6, 5, 5, 5, 5, 5, 5, 6, 
    5, 5, 5, 5, 5, 5, 5, 5, 
    5, 5, 7, 7, 7, 7, 5, 5, 
    5, 5, 7, 9, 9, 7, 5, 5, 
    5, 5, 7, 9, 9, 7, 5, 5, 
    5, 5, 7, 7, 7, 7, 5, 5, 
    5, 5, 5, 5, 5, 5, 5, 5, 
    6, 5, 5, 5, 5, 5, 5, 6
};

U64 bishop_masks[64];
U64 bishop_attacks[64][512];

//Generate bishop attacks
U64 mask_bishop_attacks(int square){
    U64 attacks = 0ULL;
    int rank, file;
    int trank = square / 8;
    int tfile = square % 8;

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

//Generate bishop attacks during a game
U64 bishop_attacks_game(int square, U64 block){
    U64 attacks = 0ULL;
    int rank, file;
    int trank = square / 8;
    int tfile = square % 8;

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

static inline U64 get_bishop_attacks(int square, U64 occupancy){
    occupancy &= bishop_masks[square];
    occupancy *= bishop_magics[square];
    occupancy >>= 64-bishop_bits[square];
    return bishop_attacks[square][occupancy];
}

//Rook relevant occupancy bits
const int rook_bits[64] = {
    12, 11, 11, 11, 11, 11, 11, 12, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    12, 11, 11, 11, 11, 11, 11, 12
};

//Rook attack mask
U64 rook_masks[64];
U64 rook_attacks[64][4096];


//Generate rook attacks
U64 mask_rook_attacks(int square){
    U64 attacks = 0ULL;
    int rank, file;
    int trank = square / 8;
    int tfile = square % 8;

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

//Generate rook attacks during a game
U64 rook_attacks_game(int square, U64 block){
    U64 attacks = 0ULL;
    int rank, file;
    int trank = square / 8;
    int tfile = square % 8;

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

static inline U64 get_rook_attacks(int square, U64 occupancy){
    occupancy &= rook_masks[square];
    occupancy *= rook_magics[square];
    occupancy >>= 64-rook_bits[square];
    return rook_attacks[square][occupancy];
}


void init_piece_attack_tables(){
    init_pawn_attack_table();
    init_knight_attack_table();
    init_king_attack_table();
    init_bishop_attacks();
    init_rook_attacks();
}

int main(){
    init_piece_attack_tables();
    
    U64 occupancy = 0ULL;
    set_bit(occupancy, c5);
    set_bit(occupancy, h5);
    print_bitboard(occupancy);

    print_bitboard(get_rook_attacks(h4, occupancy));
    
    return 0;
}