#include <stdio.h>
#include <string.h>

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
    a1, b1, c1, d1, e1, f1, g1, h1, no_sq
};

enum{
    white, black, both
};

enum {P, N, B, R, Q, K, p, n, b, r, q, k};


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

//ASCII pieces
char ascii_pieces[12] = "PNBRQKpnbrqk";

#define starting_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - "

int char_pieces[] = {
    ['P'] = P,
    ['N'] = N,
    ['B'] = B,
    ['R'] = R,
    ['Q'] = Q,
    ['K'] = K,
    ['p'] = p,
    ['n'] = n,
    ['b'] = b,
    ['r'] = r,
    ['q'] = q,
    ['k'] = k
};
//Piece bitboards
U64 bitboards[12];

//Occupancy bitboards
U64 occupancies[3];

int side;
//Enpassant square
int enpassant = no_sq;

int castle;

/*
    Castling representation
    0001 white can castle kingside
    0010 white can castle queenside
    0100 black can castle kingside
    1000 black can castle queenside
*/
//wkc = white kingsde castle
//wqc = white queenside castle
//bkc = black kingsde castle
//wqc = black queenside castle


enum {wkc=1, wqc=2, bkc=4, bqc=8};
 

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
static inline int get_ls1b_index(U64 bitboard) {
    if (bitboard == 0){
        return -1;
    }

    U64 isolated = bitboard & -bitboard;
    U64 debruijn = (isolated * 0x03f79d71b4cb0a89ULL) >> 58;
    return index64[debruijn];
}

//Print the given bitboard
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

    //Decimal representation of bitboard
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
    //Board files
    printf("    a b c d e f g h\n\n");
    //Side to move
    printf("Side:  %s\n", !side ? "white" : "black");
    //Enpasssant square
    printf("Enpassant:  %s\n", (enpassant != no_sq) ? square_to_coords[enpassant]: "no");
    //Castling rights
    printf("Castling: %c%c%c%c\n\n", (castle & wkc) ? 'K' : '-', (castle & wqc) ? 'Q' : '-', (castle & bkc) ? 'k' : '-', (castle & bqc) ? 'q' : '-');
}

//Convert a given FEN to board position
void parse_fen(char* fen){
    //Reset board and occupancies
    memset(bitboards, 0ULL, sizeof(bitboards));
    memset(occupancies, 0ULL, sizeof(occupancies));
    //Reset variables
    side = 0;
    enpassant = no_sq;
    castle = 0;

    for(int rank = 0; rank < 8; rank ++){
        for(int file = 0; file < 8; file++){
            int square = rank * 8 + file;
            //Match piece from FEN
            if((*fen >= 'a' && *fen <= 'z') || (*fen >= 'A' && *fen <= 'Z')){
                int piece = char_pieces[*fen];
                set_bit(bitboards[piece], square);
                fen++;
            }
            //Match empty space from FEN
            if(*fen >= '0' && *fen <= '9'){

                //Convert char 0 to int 0
                int offset = *fen - '0';

                int piece=-1;
                for(int bb_piece=P; bb_piece <= k; bb_piece++){
                    if(get_bit(bitboards[bb_piece], square)){
                        piece = bb_piece;
                    }
                }
                //If no piece on square
                if(piece == -1){
                    file--;
                }

                //Adjust file counter
                file += offset;

                fen++;
            }

            if(*fen == '/'){
                fen++;
            }

        }
    }
    //Move to side to move
    fen++;
    (*fen == 'w') ? (side = white) : (side = black);

    //Move to castling rights
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
    //Move to enpassant square
    fen++;

    if(*fen != '-'){
        int file = fen[0] - 'a';
        int rank = 8 - (fen[1] - '0');

        enpassant = rank * 8 + file;
    }
    else{
        enpassant = no_sq;
    }

    //Loop over white piece bitboards
    for(int piece = P; piece <= K; piece++){
        occupancies[white] |= bitboards[piece];
    }
    //Loop over black piece bitboards
    for(int piece = p; piece <= k; piece++){
        occupancies[black] |= bitboards[piece];
    }

    occupancies[both] = occupancies[white] | occupancies[black];

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
        if((bitboard >> 7) & not_a_file){
            attacks |= (bitboard >> 7);
        }
        if((bitboard >> 9) & not_h_file){
            attacks |= (bitboard >> 9);
        }
    }
    else{
        if((bitboard << 7) & not_h_file){
            attacks |= (bitboard << 7);
        }
        if((bitboard << 9) & not_a_file){
            attacks |= (bitboard << 9);
        }
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
    if((bitboard >> 7) & not_a_file){
        attacks |= (bitboard >> 7);
    }
    //Move up one
    if((bitboard >> 8)){
        attacks |= (bitboard >> 8);
    }
    //Move diagonally up left 1
    if((bitboard >> 9) & not_h_file){
        attacks |= (bitboard >> 9);
    }
    //Move diagonally down left 1
    if((bitboard << 7) & not_h_file){
        attacks |= (bitboard << 7);
    }
    //Move down 1
    if((bitboard << 8)){
        attacks |= (bitboard << 8);
    }
    //Move diagonally down right 1
    if((bitboard << 9) & not_a_file){
        attacks |= (bitboard << 9);
    }
    //Move left 1
    if((bitboard << 1) & not_a_file){
        attacks |= (bitboard << 1);
    }
    //Move right 1
    if((bitboard >> 1) & not_h_file){
        attacks |= (bitboard >> 1);
    }
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
        int square = get_ls1b_index(attack_mask);
        rem_bit(attack_mask, square);
        if(index & (1<<count)){
            occupancy |= (1ULL << square);
        }
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

static inline U64 get_queen_attacks(int square, U64 occupancy){
    
    U64 queen_attacks = 0ULL;

    U64 bishop_occupancy = occupancy;
    U64 rook_occupancy = occupancy;

    //Get bishop attacks
    bishop_occupancy &= bishop_masks[square];
    bishop_occupancy *= bishop_magics[square];
    bishop_occupancy >>= 64-bishop_bits[square];
    queen_attacks = bishop_attacks[square][bishop_occupancy];
    
    //Get rook attacks
    rook_occupancy &= rook_masks[square];
    rook_occupancy *= rook_magics[square];
    rook_occupancy >>= 64-rook_bits[square];
    queen_attacks |= rook_attacks[square][rook_occupancy];

    return queen_attacks;
}


void init_piece_attack_tables(){
    init_pawn_attack_table();
    init_knight_attack_table();
    init_king_attack_table();
    init_bishop_attacks();
    init_rook_attacks();
}

static inline int is_square_attacked(int square, int side){
    //Attacked by white pawns by reversing black pawn attacks
    if((side == white) && (pawn_attacks[black][square] & bitboards[P])){
        return 1;
    }
    //Attacked by black pawns by reversing white pawn attacks
    if((side == black) && (pawn_attacks[white][square] & bitboards[p])){
        return 1;
    }
    //Attacked by knights by reversing opposite color
    if(knight_attacks[square] & ((side == white) ? bitboards[N] : bitboards[n])){
        return 1;
    }
    //Attacked by bishops by reversing opposite color
    if(get_bishop_attacks(square, occupancies[both]) & ((side == white) ? bitboards[B] : bitboards[b])){
        return 1;
    }
    //Attacked by rooks by reversing opposite color
    if(get_rook_attacks(square, occupancies[both]) & ((side == white) ? bitboards[R] : bitboards[r])){
        return 1;
    }
    //Attacked by queens by reversing opposite color
    if(get_queen_attacks(square, occupancies[both]) & ((side == white) ? bitboards[Q] : bitboards[q])){
        return 1;
    }
    //Attacked by kings by reversing opposite color
    if(king_attacks[square] & ((side == white) ? bitboards[K] : bitboards[k])){
        return 1;
    }
    //Fefault not attacked
    return 0;
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

typedef struct{
    //Max is ~218
    int moves[256];
    //Number of moves
    int count;
} moves;

/*
                binary representation                   hexidecimal constants
  0000 0000 0000 0000 0011 1111    source square        0x3f
  0000 0000 0000 1111 1100 0000    target square        0xfc0
  0000 0000 1111 0000 0000 0000    piece                0xf000
  0000 1111 0000 0000 0000 0000    promoted piece       0xf0000
  0001 0000 0000 0000 0000 0000    capture flag         0x100000
  0010 0000 0000 0000 0000 0000    double push flag     0x200000
  0100 0000 0000 0000 0000 0000    enpassant flag       0x400000
  1000 0000 0000 0000 0000 0000    castling flag        0x800000
*/

#define encode_move(source, target, piece, promoted, capture, double, enpassant, castling) \
(source) | (target << 6) | (piece << 12) | (promoted << 16) | (capture << 20) | (double << 21) | (enpassant << 22) | (castling << 23)
 
#define get_source(move) (move & 0x3f)
#define get_target(target) ((move & 0xfc0) >> 6)
#define get_piece(piece) ((move & 0xf000) >> 12)
#define get_promoted(promoted) ((move & 0xf0000) >> 16)
#define get_capture(capture) ((move & 0x100000) >> 20)
#define get_double(double) ((move & 0x200000) >> 21)
#define get_enpassant(enpassant) ((move & 0x400000) >> 22)
#define get_castling(castling) ((move & 0x800000) >> 23)

static inline void add_move(moves *move_list, int move){
    move_list->moves[move_list->count] = move;
    move_list->count++;
}

char promoted_pieces[] = {
    [Q] = 'q',
    [R] = 'r',
    [B] = 'b',
    [N] = 'n',
    [q] = 'q',
    [r] = 'r',
    [b] = 'b',
    [n] = 'n'
};

#define copy_board() \
    U64 bitboards_copy[12], occupancies_copy[3]; \
    int side_copy, enpassant_copy, castle_copy; \
    memcpy(bitboards_copy, bitboards, 96); \
    memcpy(occupancies_copy, occupancies, 24); \
    side_copy = side, enpassant_copy = enpassant, castle_copy = castle; \

#define take_back() \
    memcpy(bitboards, bitboards_copy, 96); \
    memcpy(occupancies, occupancies_copy, 24); \
    side = side_copy, enpassant = enpassant_copy, castle = castle_copy; \

enum{all_moves, captures};

static inline int make_move(int move, int move_type){
    //Non-Captures
    if(move_type == all_moves){
        //Save board state
        copy_board();
        //Board variables
        int source_square = get_source(move);
        int target_square = get_target(move);
        int piece = get_piece(move);
        int promoted_piece = get_promoted(move);
        int capture = get_capture(move);
        int double_push = get_double(move);
        int ep = get_enpassant(move);
        int castling = get_castling(move);

        //Move piece
        rem_bit(bitboards[piece], source_square);
        set_bit(bitboards[piece], target_square);

        if(capture){
            int start_piece, end_piece;

            if(side == white){
                start_piece = p;
                end_piece = k;
            }
            else{
                start_piece = P;
                end_piece = K;
            }
            //Loop over the opposite side's bitboards to remove the captured piece
            for(int piece = start_piece; piece <= end_piece; piece++){
                //If piece on target square
                if(get_bit(bitboards[piece], target_square)){
                    //Remove the piece
                    rem_bit(bitboards[piece], target_square);
                    break;
                }
            }
        }
        //Pawn promotions
        if(promoted_piece){
            rem_bit(bitboards[side == white ? P :p], target_square);
            set_bit(bitboards[promoted_piece], target_square);
        }

        if(ep){
            //Remove pawn captured by e.p.
            (side == white) ? rem_bit(bitboards[P], target_square + 8) : rem_bit(bitboards[P], target_square - 8);
        }
        //Reset enpassant square after move
        enpassant = no_sq;

        //Make enpassant square available on double pawn pushes
        if(double_push){
            (side == white) ? (enpassant = target_square + 8) : (enpassant = target_square - 8);
        }

    }
    //Captures
    else if(get_capture(move)){
        make_move(move, all_moves);
    }
    //Illegal Moves
    else{
        return 0;
    }

}

static inline void generate_moves(moves *move_list){
    move_list->count = 0;
    //Where a piece is
    int source_square;
    //Where it is heading
    int target_square;

    //Define a piece's bitboard copy and it's attacks
    U64 bitboard;
    U64 attacks;

    for(int piece = P; piece <= k; piece++){
        //Get a copy of the piece bitboard
        bitboard = bitboards[piece];
        //White pawn moves
        if(piece == P && side == white){
            U64 single_push = (bitboard >> 8) & (~occupancies[both]); 
            //Single pawn pushes
            while(single_push){
                target_square = get_ls1b_index(single_push);
                source_square = target_square + 8;
                //Pawn promotions
                if(target_square >= a8 && target_square <= h8){
                    add_move(move_list, encode_move(source_square, target_square, P, Q, 0, 0, 0, 0));
                    add_move(move_list, encode_move(source_square, target_square, P, R, 0, 0, 0, 0));
                    add_move(move_list, encode_move(source_square, target_square, P, B, 0, 0, 0, 0));
                    add_move(move_list, encode_move(source_square, target_square, P, N, 0, 0, 0, 0));
                }
                else{
                    add_move(move_list, encode_move(source_square, target_square, P, 0, 0, 0, 0, 0));
                    //Double pawn moves
                    if (source_square >= a2 && source_square <= h2) {
                        int double_push = source_square - 16;
                        if (!get_bit(occupancies[both], double_push))
                            add_move(move_list, encode_move(source_square, double_push, P, 0, 0, 1, 0, 0));
                    }
                }
                rem_bit(single_push, target_square); 
            }
            //Pawn attacks
            while(bitboard){
                source_square = get_ls1b_index(bitboard);
            
                attacks = pawn_attacks[white][source_square] & occupancies[black];
                while(attacks){
                    target_square = get_ls1b_index(attacks);
                    if(target_square >= a8 && target_square <= h8){
                        add_move(move_list, encode_move(source_square, target_square, P, Q, 1, 0, 0, 0));
                        add_move(move_list, encode_move(source_square, target_square, P, R, 1, 0, 0, 0));
                        add_move(move_list, encode_move(source_square, target_square, P, B, 1, 0, 0, 0));
                        add_move(move_list, encode_move(source_square, target_square, P, N, 1, 0, 0, 0));
                    }
                    else{
                        add_move(move_list, encode_move(source_square, target_square, P, 0, 1, 0, 0, 0));
                    }
                    rem_bit(attacks, target_square);
                }
                if(enpassant != no_sq){
                    U64 enpassant_attacks = pawn_attacks[white][source_square] & (1ULL << enpassant);
                    if(enpassant_attacks){
                        int target_enpassant = get_ls1b_index(enpassant_attacks);
                        add_move(move_list, encode_move(source_square, target_enpassant, P, 0, 1, 0, 1, 0));
                    }
                }
                rem_bit(bitboard, source_square);
            }
        }

        //Generate black pawn moves
        if(piece == p && side == black){
            //Single pawn pushes
            U64 single_push = (bitboard << 8) & (~occupancies[both]);
            while(single_push){
                target_square = get_ls1b_index(single_push);
                source_square = target_square - 8;
                //Pawn promotions
                if(target_square >= a1 && target_square <= h1){
                    add_move(move_list, encode_move(source_square, target_square, p, q, 1, 0, 0, 0));
                    add_move(move_list, encode_move(source_square, target_square, p, r, 1, 0, 0, 0));
                    add_move(move_list, encode_move(source_square, target_square, p, b, 1, 0, 0, 0));
                    add_move(move_list, encode_move(source_square, target_square, p, n, 1, 0, 0, 0));
                }
                else{
                    add_move(move_list, encode_move(source_square, target_square, p, 0, 1, 0, 0, 0));
                    //Double pawn moves
                    if (source_square >= a7 && source_square <= h7) {
                        int double_push = source_square + 16;
                        if (!get_bit(occupancies[both], double_push))
                            add_move(move_list, encode_move(source_square, double_push, p, 0, 0, 1, 0, 0));
                    }
                }
                rem_bit(single_push, target_square);
            }
            while(bitboard){
                source_square = get_ls1b_index(bitboard);
            
                attacks = pawn_attacks[black][source_square] & occupancies[white];
                while(attacks){
                    target_square = get_ls1b_index(attacks);
                    if(target_square >= a1 && target_square <= h1){
                        add_move(move_list, encode_move(source_square, target_square, p, q, 1, 0, 0, 0));
                        add_move(move_list, encode_move(source_square, target_square, P, r, 1, 0, 0, 0));
                        add_move(move_list, encode_move(source_square, target_square, P, b, 1, 0, 0, 0));
                        add_move(move_list, encode_move(source_square, target_square, P, n, 1, 0, 0, 0));
                    }
                    else{
                        add_move(move_list, encode_move(source_square, target_square, p, 0, 1, 0, 0, 0));
                    }
                    rem_bit(attacks, target_square);
                }
                if(enpassant != no_sq){
                    U64 enpassant_attacks = pawn_attacks[black][source_square] & (1ULL << enpassant);
                    if(enpassant_attacks){
                        int target_enpassant = get_ls1b_index(enpassant_attacks);
                        add_move(move_list, encode_move(source_square, target_enpassant, p, 0, 1, 0, 1, 0));
                    }
                }
                rem_bit(bitboard, source_square);
            }
        }

        if(piece == K && side == white){
            //Kingside castling is legal
            if(castle & wkc){
                if(!get_bit(occupancies[both], f1) && !get_bit(occupancies[both], g1) && !is_square_attacked(f1, black) && !is_square_attacked(g1, black)){
                    add_move(move_list, encode_move(e1, g1, K, 0, 0, 0, 0, 1));
                }
            }
            //Queenside castling is legal
            if(castle & wqc){
                if(!get_bit(occupancies[both], d1) && !get_bit(occupancies[both], c1) && !is_square_attacked(d1, black) && !is_square_attacked(c1, black)){
                    add_move(move_list, encode_move(e1, c1, K, 0, 0, 0, 0, 1));
                }
            }
        }

        if(piece == k && side == black){
            //Kingside castling is legal
            if(castle & bkc){
                if(!get_bit(occupancies[both], f8) && !get_bit(occupancies[both], g8) && !is_square_attacked(f8, white) && !is_square_attacked(g8, white)){
                    add_move(move_list, encode_move(e8, g8, k, 0, 0, 0, 0, 1));
                }
            }
            //Queenside castling is legal
            if(castle & bqc){
                if(!get_bit(occupancies[both], d8) && !get_bit(occupancies[both], c8) && !is_square_attacked(d8, white) && !is_square_attacked(c8, white)){
                    add_move(move_list, encode_move(e8, c8, k, 0, 0, 0, 0, 1));
                }
            }
        }

        //Generate white knight moves
        if(piece == N && side == white){
            while(bitboard){
                source_square = get_ls1b_index(bitboard);
                //Get knight attacks for the specific piece without landing on own
                attacks = knight_attacks[source_square] & ~occupancies[white];
                while(attacks){
                    target_square = get_ls1b_index(attacks);
                    //Non-capture
                    if(!get_bit(occupancies[black], target_square)){
                        add_move(move_list, encode_move(source_square, target_square, N, 0, 0, 0, 0, 0));
                    }
                    //Capture
                    else{
                        add_move(move_list, encode_move(source_square, target_square, N, 0, 1, 0, 0, 0));
                    }
                    rem_bit(attacks, target_square);
                }
                rem_bit(bitboard, source_square);
            }
        }
        //Generate black knight moves
        if(piece == n && side == black){
            while(bitboard){
                source_square = get_ls1b_index(bitboard);
                //Get knight attacks for the specific piece without landing on own
                attacks = knight_attacks[source_square] & ~occupancies[black];
                while(attacks){
                    target_square = get_ls1b_index(attacks);
                    //Non-capture
                    if(!get_bit(occupancies[white], target_square)){
                        add_move(move_list, encode_move(source_square, target_square, n, 0, 0, 0, 0, 0));
                    }
                    //Capture
                    else{
                        add_move(move_list, encode_move(source_square, target_square, n, 0, 1, 0, 0, 0));
                    }
                    rem_bit(attacks, target_square);
                }
                rem_bit(bitboard, source_square);
            }
        }

        //Generate white bishop moves
        if(piece == B && side == white){
            while(bitboard){
                source_square = get_ls1b_index(bitboard);
                //Get bishop attacks for the specific piece
                attacks = get_bishop_attacks(source_square, occupancies[both]) & ~occupancies[white];
                while(attacks){
                    target_square = get_ls1b_index(attacks);
                    //Non-capture
                    if(!get_bit(occupancies[black], target_square)){
                        add_move(move_list, encode_move(source_square, target_square, B, 0, 0, 0, 0, 0));
                    }
                    //Capture
                    else{
                        add_move(move_list, encode_move(source_square, target_square, B, 0, 1, 0, 0, 0));
                    }
                    rem_bit(attacks, target_square);
                }
                rem_bit(bitboard, source_square);
            }
        }
        //Generate black bishop moves
        if(piece == b && side == black){
            while(bitboard){
                source_square = get_ls1b_index(bitboard);
                //Get bishop attacks for the specific piece
                attacks = get_bishop_attacks(source_square, occupancies[both]) & ~occupancies[black];
                while(attacks){
                    target_square = get_ls1b_index(attacks);
                    //Non-capture
                    if(!get_bit(occupancies[white], target_square)){
                        add_move(move_list, encode_move(source_square, target_square, b, 0, 0, 0, 0, 0));
                    }
                    //Capture
                    else{
                        add_move(move_list, encode_move(source_square, target_square, b, 0, 1, 0, 0, 0));
                    }
                    rem_bit(attacks, target_square);
                }
                rem_bit(bitboard, source_square);
            }
        }

        //Generate white rook moves
        if(piece == R && side == white){
            while(bitboard){
                source_square = get_ls1b_index(bitboard);
                //Get rook attacks for the specific piece
                attacks = get_rook_attacks(source_square, occupancies[both]) & ~occupancies[white];
                while(attacks){
                    target_square = get_ls1b_index(attacks);
                    //Non-capture
                    if(!get_bit(occupancies[black], target_square)){
                        add_move(move_list, encode_move(source_square, target_square, R, 0, 0, 0, 0, 0));
                    }
                    //Capture
                    else{
                        add_move(move_list, encode_move(source_square, target_square, R, 0, 1, 0, 0, 0));
                    }
                    rem_bit(attacks, target_square);
                }
                rem_bit(bitboard, source_square);
            }
        }
        //Generate black rook moves
        if(piece == r && side == black){
            while(bitboard){
                source_square = get_ls1b_index(bitboard);
                //Get rook attacks for the specific piece
                attacks = get_rook_attacks(source_square, occupancies[both]) & ~occupancies[black];
                while(attacks){
                    target_square = get_ls1b_index(attacks);
                    //Non-capture
                    if(!get_bit(occupancies[white], target_square)){
                        add_move(move_list, encode_move(source_square, target_square, r, 0, 0, 0, 0, 0));
                    }
                    //Capture
                    else{
                        add_move(move_list, encode_move(source_square, target_square, r, 0, 1, 0, 0, 0));
                    }
                    rem_bit(attacks, target_square);
                }
                rem_bit(bitboard, source_square);
            }
        }
        //Generate white queen moves
        if(piece == Q && side == white){
            while(bitboard){
                source_square = get_ls1b_index(bitboard);
                //Get queen attacks for the specific piece
                attacks = get_queen_attacks(source_square, occupancies[both]) & ~occupancies[white];
                while(attacks){
                    target_square = get_ls1b_index(attacks);
                    //Non-capture
                    if(!get_bit(occupancies[black], target_square)){
                        add_move(move_list, encode_move(source_square, target_square, Q, 0, 0, 0, 0, 0));
                    }
                    //Capture
                    else{
                        add_move(move_list, encode_move(source_square, target_square, Q, 0, 1, 0, 0, 0));
                    }
                    rem_bit(attacks, target_square);
                }
                rem_bit(bitboard, source_square);
            }
        }
        //Generate black queen moves
        if(piece == q && side == black){
            while(bitboard){
                source_square = get_ls1b_index(bitboard);
                //Get black attacks for the specific piece
                attacks = get_queen_attacks(source_square, occupancies[both]) & ~occupancies[black];
                while(attacks){
                    target_square = get_ls1b_index(attacks);
                    //Non-capture
                    if(!get_bit(occupancies[white], target_square)){
                        add_move(move_list, encode_move(source_square, target_square, q, 0, 0, 0, 0, 0));
                    }
                    //Capture
                    else{
                        add_move(move_list, encode_move(source_square, target_square, q, 0, 1, 0, 0, 0));
                    }
                    rem_bit(attacks, target_square);
                }
                rem_bit(bitboard, source_square);
            }
        }

        //Generate white king moves
        if(piece == K && side == white){
            while(bitboard){
                source_square = get_ls1b_index(bitboard);
                //Get king attacks for the specific piece without landing on own
                attacks = king_attacks[source_square] & ~occupancies[white];
                while(attacks){
                    target_square = get_ls1b_index(attacks);
                    //Non-capture
                    if(!get_bit(occupancies[black], target_square)){
                        add_move(move_list, encode_move(source_square, target_square, K, 0, 0, 0, 0, 0));
                    }
                    //Capture
                    else{
                        add_move(move_list, encode_move(source_square, target_square, K, 0, 1, 0, 0, 0));
                    }
                    rem_bit(attacks, target_square);
                }
                rem_bit(bitboard, source_square);
            }
        }

        //Generate black king moves
        if(piece == k && side == black){
            while(bitboard){
                source_square = get_ls1b_index(bitboard);
                //Get king attacks for the specific piece without landing on own
                attacks = king_attacks[source_square] & ~occupancies[black];
                while(attacks){
                    target_square = get_ls1b_index(attacks);
                    //Non-capture
                    if(!get_bit(occupancies[white], target_square)){
                        add_move(move_list, encode_move(source_square, target_square, k, 0, 0, 0, 0, 0));
                    }
                    //Capture
                    else{
                        add_move(move_list, encode_move(source_square, target_square, k, 0, 1, 0, 0, 0));
                    }
                    rem_bit(attacks, target_square);
                }
                rem_bit(bitboard, source_square);
            }
        }



    }
}

//For UCI
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

int main(){
    init_piece_attack_tables();

    parse_fen("8/8/8/8/8/8/pPpPpPpP/8 w - - 0 1");
    print_board();
    
    moves move_list[1];

    generate_moves(move_list);

    for(int count = 0; count < move_list->count; count++){
        int move = move_list->moves[count];

        copy_board();

        make_move(move, all_moves);
        print_board();
        getchar();
        take_back();
        print_board();
        getchar();
    }


}