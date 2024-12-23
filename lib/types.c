#include "types.h"

// clang-format off
const char* square_to_string[] = {
  "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
  "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
  "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
  "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
  "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
  "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
  "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
  "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
};
// clang-format on

const char* piece_to_name[] = {
  "-", "wp", "wn", "wb", "wr", "wq", "wk", "-",
  "-", "bp", "bn", "bb", "br", "bq", "bk",
};

const char* piece_to_string[] = {
  " ", "♟", "♞", "♝", "♜", "♛", "♚", " ", " ", "♙", "♘", "♗", "♖", "♕", "♔",
};

const char* piece_to_char = " PNBRQK  pnbrqk";
constexpr size_t piece_to_char_size = 15;
int scope = 0;

size_t find_piece(char token) {
  for (size_t i = 0; i < piece_to_char_size; ++i) {
    if (piece_to_char[i] == token) {
      return i;
    }
  }
  return PIECE_NOT_FOUND;
}

uint32_t xor_shift_random32() {
  static uint32_t state = 3574335;
  uint32_t n = state;
  n ^= n << 13;
  n ^= n >> 17;
  n ^= n << 5;
  state = n;
  return n;
}

uint64_t get_random() {
  uint64_t n1 = xor_shift_random32() & 0xffff;
  uint64_t n2 = xor_shift_random32() & 0xffff;
  uint64_t n3 = xor_shift_random32() & 0xffff;
  uint64_t n4 = xor_shift_random32() & 0xffff;
  return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}
