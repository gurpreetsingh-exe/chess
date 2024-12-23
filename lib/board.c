#include "board.h"
#include "movegen.h"

Piece board[SQ_NB];
Color side_to_move;
int enpassant = -1;
uint8_t castling = 0b1111;
Bitboard piece[PIECE_NB];
Bitboard type[PIECE_TYPE_NB];
Bitboard color_bb[COLOR_NB];
Bitboard pins[COLOR_NB];
Bitboard pinners[COLOR_NB];
Bitboard checkers_bb;
Bitboard attacked_bb;
Moves prev_moves;

StateInfo state_info[MAX_MOVES_COUNT];
size_t si_current_idx = 0;

#define UNSET_CASTLING(c)                                                      \
  if (castling & (c)) {                                                        \
    castling ^= (c);                                                           \
  }

const char* start_fen =
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

// const char* start_fen = "rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w";

// const char* start_fen =
//     "2b5/p2NBp1p/1bp1nPPr/3P4/2pRnr1P/1k1B1Ppp/1P1P1pQP/Rq1N3K b - - 0 1";

// const char* start_fen =
//     "rnbqkbnr/pp1ppppp/8/2p5/2B1P3/5N2/PPPP1PPP/R3K2R b KQkq - 1 2";

void setup_starting_position() { set_position(start_fen); }

static void update_pins(Color c) {
  pins[c] = EMPTY;
  pinners[c ^ BLACK] = EMPTY;
  Square king_sq = find_square(PTY_KING, c);
  Bitboard pinner =
      ((attacks_bb(PTY_ROOK, king_sq, 0) & (type[PTY_QUEEN] | type[PTY_ROOK])) |
       (attacks_bb(PTY_BISHOP, king_sq, 0) &
        (type[PTY_QUEEN] | type[PTY_BISHOP]))) &
      color_bb[c ^ BLACK];
  Bitboard occ = type[ALL_PIECES] ^ pinner ^ piece[MAKE_PC(PTY_KING, c)];
  pins[c] = EMPTY;
  while (pinner) {
    int sq = pop_lsb(&pinner);
    Bitboard b = between_bb(sq, king_sq) & occ;
    if (b && !MORE_THAN_ONE(b)) {
      pins[c] |= b;
      if (b & color_bb[c]) {
        pinners[c ^ BLACK] |= SQBB(sq);
      }
    }
  }
}

void update_check_info() {
  Color us = side_to_move;
  Color them = side_to_move ^ BLACK;
  update_pins(WHITE);
  update_pins(BLACK);
  checkers_bb =
      attackers_to(PC_SQUARE(PTY_KING, them), type[ALL_PIECES]) & color_bb[us];
}

void update_state() {
  attacked_bb = EMPTY;
  for (int pc = 0; pc < PIECE_NB; ++pc) {
    if (PC_COLOR(pc) != side_to_move ^ BLACK) {
      continue;
    }
    Bitboard bb = piece[pc];
    while (bb) {
      int sq = pop_lsb(&bb);
      Piece p = board[sq];
      if (PC_TYPE(p) == PTY_PAWN) {
        attacked_bb |= pawn_attacks(sq, side_to_move ^ BLACK);
      } else {
        attacked_bb |= attacks_bb(PC_TYPE(p), sq,
                                  OCCUPIED_SQUARES &
                                      ~piece[MAKE_PC(PTY_KING, side_to_move)]);
      }
    }
  }
}

void put_piece(Piece pc, Square square) {
  board[square] = pc;
  Bitboard sqbb = SQBB(square);
  piece[pc] |= sqbb;
  type[ALL_PIECES] |= type[PC_TYPE(pc)] |= sqbb;
  color_bb[pc >> 3] |= sqbb;
}

Piece take_piece(Square square) {
  Piece pc = board[square];
  board[square] = NO_PIECE;
  POP_BIT(piece[pc], square);
  POP_BIT(type[ALL_PIECES], square);
  POP_BIT(type[PC_TYPE(pc)], square);
  POP_BIT(color_bb[pc >> 3], square);
  return pc;
}

Castling square_to_castling_side(Square sq) {
  PUSH_IGNORE_WARNING("-Wswitch-enum")
  switch (sq) {
    case a1: return CASTLE_W_QUEENSIDE;
    case h1: return CASTLE_W_KINGSIDE;
    case a8: return CASTLE_B_QUEENSIDE;
    case h8: return CASTLE_B_KINGSIDE;
    default: return NO_CASTLE;
  }
  POP_IGNORE_WARNING()
}

Square find_square(PieceType pt, Color c) {
  Bitboard bb = piece[MAKE_PC(pt, c)];
  return pop_lsb(&bb);
}

void make_move(Move move) {
  int from = MOVE_FROM(move);
  int to = MOVE_TO(move);
  int flags = MOVE_FLAGS(move);
  Color us = side_to_move;
  Color them = side_to_move ^ BLACK;
  SICURR.castling = castling;
  enpassant = -1;
  SICURR.enpassant = -1;
  SICURR.captured = NO_PIECE;
  switch (flags) {
    case CASTLING: {
      bool king_side = (from - to) < 0;
      put_piece(take_piece((side_to_move * a8) + 7 * king_side),
                (side_to_move ? d8 : d1) + 2 * king_side);
      UNSET_CASTLING(side_to_move ? (CASTLE_B_KINGSIDE | CASTLE_B_QUEENSIDE)
                                  : (CASTLE_W_KINGSIDE | CASTLE_W_QUEENSIDE))
    } break;
    case EN_PASSANT: {
      Square sq = to + (side_to_move ? 8 : -8);
      SICURR.captured = take_piece(sq);
    } break;
    case PROMOTION: {
      assert(false && "not implemented");
    } break;
    case NORMAL: break;
  }
  Piece pc = take_piece(from);
  if (PC_TYPE(pc) == PTY_ROOK) {
    UNSET_CASTLING(square_to_castling_side(from));
  } else if (PC_TYPE(pc) == PTY_KING) {
    UNSET_CASTLING(side_to_move ? (CASTLE_B_KINGSIDE | CASTLE_B_QUEENSIDE)
                                : (CASTLE_W_KINGSIDE | CASTLE_W_QUEENSIDE));
  } else if (PC_TYPE(pc) == PTY_PAWN) {
    if ((to ^ from) == 16) {
      Square sq = from + (side_to_move ? -8 : 8);
      if (pawn_attacks(sq, us) & piece[MAKE_PC(PTY_PAWN, them)]) {
        enpassant = sq;
        SICURR.enpassant = sq;
      }
    }
  }

  if (!flags && board[to]) {
    Piece captured = take_piece(to);
    SICURR.captured = captured;
    if (PC_TYPE(captured) == PTY_ROOK) {
      UNSET_CASTLING(square_to_castling_side(to));
    }
  }
  put_piece(pc, to);
  update_check_info();
  side_to_move = side_to_move ^ BLACK;
  update_state();
  MOVE_APPEND(&prev_moves, move);
  si_current_idx++;
}

void unmake_move(Move move) {
  int from = MOVE_FROM(move);
  int to = MOVE_TO(move);
  int flags = MOVE_FLAGS(move);
  side_to_move = side_to_move ^ BLACK;
  prev_moves.end--;
  si_current_idx--;
  Piece pc = take_piece(to);
  put_piece(pc, from);
  Piece captured = SICURR.captured;
  if (captured && flags != EN_PASSANT) {
    put_piece(captured, to);
  }
  if (flags == CASTLING) {
    bool king_side = (from - to) < 0;
    Piece rook = MAKE_PC(PTY_ROOK, side_to_move);
    if (side_to_move) {
      take_piece(king_side ? f8 : d8);
      put_piece(rook, king_side ? h8 : a8);
    } else {
      take_piece(king_side ? f1 : d1);
      put_piece(rook, king_side ? h1 : a1);
    }
  } else if (flags == EN_PASSANT) {
    Square sq = to + (side_to_move ? 8 : -8);
    put_piece(captured, sq);
  }
  castling = SICURR.castling;
  enpassant =
      si_current_idx >= 1 ? state_info[si_current_idx - 1].enpassant : -1;
  update_check_info();
  update_state();
}

Castling char_to_castling(char c) {
  switch (c) {
    case 'K': return CASTLE_W_KINGSIDE;
    case 'Q': return CASTLE_W_QUEENSIDE;
    case 'k': return CASTLE_B_KINGSIDE;
    case 'q': return CASTLE_B_QUEENSIDE;
    default: return NO_CASTLE;
  }
}

void reset_position() {
  memset(board, 0, sizeof(board));
  side_to_move = WHITE;
  enpassant = -1;
  castling = 0b1111;
  memset(piece, 0, sizeof(piece));
  memset(type, 0, sizeof(type));
  memset(color_bb, 0, sizeof(color_bb));
  memset(pins, 0, sizeof(pins));
  memset(pinners, 0, sizeof(pinners));
  checkers_bb = EMPTY;
  attacked_bb = EMPTY;

  memset(state_info, 0, sizeof(state_info));
  MOVE_INIT(prev_moves);
  si_current_idx = 0;
}

void set_position(const char* fen) {
  reset_position();
  size_t i = 0;
  char token = '\0';
  int square = a8;
  size_t idx;
  size_t size = strlen(fen);
  while (i < size && (token = fen[i]) != ' ') {
    if (isdigit(token)) {
      square += (token - '0') * EAST;
    } else if (token == '/') {
      square += 2 * SOUTH;
    } else if ((idx = find_piece(token)) != PIECE_NOT_FOUND) {
      put_piece(idx, square);
      ++square;
    }
    i++;
  }
  assert(fen[i++] == ' ');
  token = fen[i++];
  if (token == 'b') {
    side_to_move = BLACK;
  } else if (token == 'w') {
    side_to_move = WHITE;
  }

  if (!fen[i++]) {
    castling = 0;
    update_check_info();
    update_state();
    return;
  }

  if (fen[i] == '-') {
    castling = NO_CASTLE;
    i++;
  } else {
    castling = NO_CASTLE;
    while ((token = fen[i++]) && token != ' ') {
      castling |= char_to_castling(token);
    }
  }

  update_check_info();
  update_state();
}

#if 0
void print_board() {
  printf("┌───┬───┬───┬───┬───┬───┬───┬───┐\n");
  for (int j = GRID_SIZE - 1; j >= 0; --j) {
    printf("│");
    for (int i = 0; i < GRID_SIZE; ++i) {
      int idx = i + j * GRID_SIZE;
      const int piece = board[idx];
      const char* bg = (i + j) % 2 ? "\x1b[43m" : "\x1b[40m";
      printf("%s %s \x1b[0m", bg, piece_to_string[piece]);
      printf("│");
    }
    if (j != 0) {
      printf("\n├───┼───┼───┼───┼───┼───┼───┼───┤\n");
    }
  }
  printf("\n└───┴───┴───┴───┴───┴───┴───┴───┘\n");
}
#elif 0
void print_board() {
  for (int j = GRID_SIZE - 1; j >= 0; --j) {
    for (int i = 0; i < GRID_SIZE; ++i) {
      int idx = i + j * GRID_SIZE;
      const int piece = board[idx];
      const char* bg = (i + j) % 2 ? "\x1b[43m" : "\x1b[40m";
      printf("%s %s \x1b[0m", bg, piece_to_string[piece]);
    }
    printf("\n");
  }
}
#else
void print_board() {
  for (int j = GRID_SIZE - 1; j >= 0; --j) {
    printf("  %d | ", j + 1);
    for (int i = 0; i < GRID_SIZE; ++i) {
      int idx = i + j * GRID_SIZE;
      const int pc = board[idx];
      printf("%s ", piece_to_string[pc]);
    }
    printf("\n");
  }
  printf("      ---------------\n");
  printf("      a b c d e f g h\n");
}
#endif
