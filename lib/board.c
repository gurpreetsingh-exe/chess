#include "board.h"

Piece board[SQ_NB];
int piece_count[PIECE_NB];
Color side_to_move;
int enpessant = -1;
uint8_t castling = 0b1111;
Bitboard piece[PIECE_NB];
Bitboard type[PIECE_TYPE_NB];
Bitboard color_bb[COLOR_NB];

// const char* start_fen =
//     "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

// const char* start_fen = "rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w";

const char* start_fen =
    "2b5/p2NBp1p/1bp1nPPr/3P4/2pRnr1P/1k1B1Ppp/1P1P1pQP/Rq1N3K b - - 0 1";

void setup_starting_position() { set_position(start_fen); }

void put_piece(Piece pc, Square square) {
  board[square] = pc;
  Bitboard sqbb = SQBB(square);
  piece[pc] |= sqbb;
  type[ALL_PIECES] |= type[PC_TYPE(pc)] |= sqbb;
  color_bb[pc >> 3] |= sqbb;
  piece_count[pc]++;
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

void set_position(const char* fen) {
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
      printf("%s \x1b[0m", piece_to_string[pc]);
    }
    printf("\n");
  }
  printf("      ---------------\n");
  printf("      a b c d e f g h\n");
}
#endif
