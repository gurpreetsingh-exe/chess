#pragma once

#include "types.h"

typedef uint64_t Bitboard;

#define GET_BIT(n, bit) ((n & (1ULL << (bit))) >> (bit))
#define SET_BIT(n, bit) (n |= (1ULL << (bit)))
#define FLIP_BIT(n, bit) (n ^= (1ULL << (bit)))
#define POP_BIT(n, bit) (GET_BIT(n, bit) ? n ^= (1ULL << (bit)) : 0)
#define MORE_THAN_ONE(n) ((n) & ((n)-1))
#define POP_LSB(n) n &= n - 1

#define SQBB(i) (1ULL << (i))

#define EMPTY 0ULL
#define UNIVERSE 0xffffffffffffffffULL
#define FILE_A 0x8080808080808080ULL
#define FILE_B (FILE_A >> 1)
#define FILE_C (FILE_A >> 2)
#define FILE_D (FILE_A >> 3)
#define FILE_E (FILE_A >> 4)
#define FILE_F (FILE_A >> 5)
#define FILE_G (FILE_A >> 6)
#define FILE_H (FILE_A >> 7)

#define FILE_AB (FILE_A | FILE_B)
#define FILE_GH (FILE_G | FILE_H)

#define RANK_1 0xffULL
#define RANK_2 (RANK_1 << 8)
#define RANK_3 (RANK_1 << 16)
#define RANK_4 (RANK_1 << 24)
#define RANK_5 (RANK_1 << 32)
#define RANK_6 (RANK_1 << 40)
#define RANK_7 (RANK_1 << 48)
#define RANK_8 (RANK_1 << 56)

#define RLEFT(bb, s) ((bb << s) | (bb >> (64 - s)))
#define RRIGHT(bb, s) ((bb >> s) | (bb << (64 - s)))

// (__builtin_popcountll)

int count_bits(Bitboard);
int lsb(Bitboard);
int pop_lsb(Bitboard*);
void print_bitboard(Bitboard);
void print_attack_bitboard(Bitboard, Bitboard);
Bitboard between_bb(Square, Square);
void init_bitboards();
Bitboard attackers_to(Square, Bitboard);
bool is_attacked(Square, Bitboard, Color);
Bitboard find_magic_number(int, int, bool);
