#pragma once

#include "bitboard.h"
#include "move.h"
#include "types.h"

extern Piece board[SQ_NB];
extern int piece_count[PIECE_NB];
extern Color side_to_move;
extern int enpassant;
extern uint8_t castling;
extern Bitboard piece[PIECE_NB];
extern Bitboard type[PIECE_TYPE_NB];
extern Bitboard color_bb[COLOR_NB];
extern Bitboard pins[COLOR_NB];
extern Bitboard pinners[COLOR_NB];
extern Bitboard checkers_bb;
extern const char* start_fen;
extern Moves prev_moves;

typedef struct StateInfo {
  Piece captured;
  Castling castling;
  int enpassant;
} StateInfo;

extern StateInfo state_info[MAX_MOVES_COUNT];
extern size_t si_current_idx;
#define SICURR state_info[si_current_idx]

#define OCCUPIED_SQUARES type[ALL_PIECES]
#define EMPTY_SQUARES ~OCCUPIED_SQUARES
#define US color_bb[side_to_move]
#define OPPONENT color_bb[side_to_move ^ BLACK]
#define PC_COLOR(pc) (pc) >> 3
#define COLOR_AT(square) PC_COLOR(board[(square)])
#define PC_SQUARE(pt, c) lsb(piece[MAKE_PC(pt, c)])

void setup_starting_position();
void put_piece(Piece, Square);
Piece take_piece(Square);
void reset_position();
void set_position(const char* /* fen */);
void make_move(Move);
void unmake_move(Move);
void print_board();
