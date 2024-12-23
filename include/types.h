#pragma once

#include <time.h>

constexpr int GRID_SIZE = 8;
constexpr int MAX_INPUT_SIZE = 128;
constexpr int MAX_MOVES_COUNT = 256;

#define PIECE_NOT_FOUND 50
#define PC_TYPE(p) (p & 7)
#define MAKE_PC(pt, c) ((c << 3) + pt)

extern int scope;

#if 0
#define TIME(name, ...)                                                        \
  do {                                                                         \
    scope++;                                                                   \
    clock_t begin = clock();                                                   \
    __VA_ARGS__;                                                               \
    clock_t end = clock();                                                     \
    double time = (double)(end - begin) * 1000.0f / CLOCKS_PER_SEC;            \
    for (int i = 1; i < scope; ++i) { printf("  "); }                          \
    printf("%s: %.3f ms\n", name, time);                                       \
    scope--;                                                                   \
  } while (0)
#else
#define TIME(name, ...) __VA_ARGS__
#endif

#define STREQ(a, b) (strncmp(a, b, MAX_INPUT_SIZE) == 0)

// #define FOREACH_SQUARE(...)                                                    \
//   for (int rank = GRID_SIZE - 1; rank >= 0; --rank) {                          \
//     for (int file = 0; file < GRID_SIZE; ++file) {                             \
//       {                                                                        \
//         int square = file + rank * GRID_SIZE;                                  \
//         __VA_ARGS__                                                            \
//       }                                                                        \
//     }                                                                          \
//   }

typedef enum PieceType {
  NO_PIECE_TYPE,
  PTY_PAWN,
  PTY_KNIGHT,
  PTY_BISHOP,
  PTY_ROOK,
  PTY_QUEEN,
  PTY_KING,
  ALL_PIECES = 0,
  PIECE_TYPE_NB = 8,
} PieceType;

typedef enum Piece : int {
  NO_PIECE,
  PC_W_PAWN = PTY_PAWN,
  PC_W_KNIGHT,
  PC_W_BISHOP,
  PC_W_ROOK,
  PC_W_QUEEN,
  PC_W_KING,
  PC_B_PAWN = PTY_PAWN + 8,
  PC_B_KNIGHT,
  PC_B_BISHOP,
  PC_B_ROOK,
  PC_B_QUEEN,
  PC_B_KING,
  PIECE_NB = 16
} Piece;

// clang-format off
typedef enum Square {
  a1, b1, c1, d1, e1, f1, g1, h1,
  a2, b2, c2, d2, e2, f2, g2, h2,
  a3, b3, c3, d3, e3, f3, g3, h3,
  a4, b4, c4, d4, e4, f4, g4, h4,
  a5, b5, c5, d5, e5, f5, g5, h5,
  a6, b6, c6, d6, e6, f6, g6, h6,
  a7, b7, c7, d7, e7, f7, g7, h7,
  a8, b8, c8, d8, e8, f8, g8, h8,

  SQ_ZERO = 0,
  SQ_NB = 64,
} Square;

enum Direction {
  NORTH = 8,
  EAST  = 1,
  SOUTH = -NORTH,
  WEST  = -EAST,

  NORTH_EAST = NORTH + EAST,
  SOUTH_EAST = SOUTH + EAST,
  SOUTH_WEST = SOUTH + WEST,
  NORTH_WEST = NORTH + WEST
};
// clang-format on

typedef enum Color {
  WHITE,
  BLACK,
  COLOR_NB = 2,
} Color;

typedef enum Castling : uint8_t {
  NO_CASTLE,
  CASTLE_W_QUEENSIDE = 1 << 1,
  CASTLE_W_KINGSIDE = 1 << 2,
  CASTLE_B_QUEENSIDE = 1 << 3,
  CASTLE_B_KINGSIDE = 1 << 4,
} Castling;

typedef enum MoveType : uint16_t {
  NORMAL = 0,
  PROMOTION = 1,
  EN_PASSANT = 2,
  CASTLING = 3
} MoveType;

extern const char* square_to_string[];
extern const char* piece_to_name[];
extern const char* piece_to_string[];
extern const char* piece_to_char;

size_t find_piece(char);
uint32_t xor_shift_random32();
uint64_t get_random();
