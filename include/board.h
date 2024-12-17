#pragma once

#include "types.h"

extern Piece board[SQ_NB];
extern int piece_count[PIECE_NB];
extern Color side_to_move;
extern int enpessant;
extern uint8_t castling;
extern Bitboard piece[PIECE_NB];
extern Bitboard type[PIECE_TYPE_NB];
extern Bitboard color_bb[COLOR_NB];
extern const char* start_fen;

#define OCCUPIED_SQUARES type[ALL_PIECES]
#define EMPTY_SQUARES ~OCCUPIED_SQUARES
#define US color_bb[side_to_move]
#define OPPONENT color_bb[1 - side_to_move]
#define PC_COLOR(pc) (pc) >> 3
#define COLOR_AT(square) PC_COLOR(board[(square)])

void setup_starting_position();
void put_piece(Piece, Square);
void set_position(const char* /* fen */);
void print_board();
