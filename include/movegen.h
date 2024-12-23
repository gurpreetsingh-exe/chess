#pragma once

#include "bitboard.h"
#include "types.h"

typedef struct Moves Moves;

void init_slider_attacks();
void init_leaper_attacks();
Bitboard pawn_attacks(Square, Color);
Bitboard knight_attacks(Square);
Bitboard bishop_attacks(Square, Bitboard);
Bitboard bishop_attacks_mask(Square);
Bitboard bishop_attacks_on_the_fly(Square, Bitboard);
Bitboard rook_attacks(Square, Bitboard);
Bitboard rook_attacks_mask(Square);
Bitboard rook_attacks_on_the_fly(Square, Bitboard);
Bitboard queen_attacks(Square, Bitboard);
Bitboard king_attacks(Square);
Bitboard attacks_bb(PieceType, Square, Bitboard);
void generate_moves(Moves*);
Bitboard get_moves(Square);
Bitboard set_occupancy(int, int, Bitboard);
void print_moves();
