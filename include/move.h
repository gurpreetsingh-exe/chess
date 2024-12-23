#pragma once

#include "types.h"

#define MOVE(from, to, flags)                                                  \
  (((flags)&0xf) << 12) | (((from)&0x3f) << 6) | ((to)&0x3f)
#define MOVE_TO(move) ((move)&0x3f)
#define MOVE_FROM(move) (((move) >> 6) & 0x3f)
#define MOVE_FLAGS(move) (((move) >> 12) & 0x3f)

typedef uint16_t Move;
void string_of_move(Move);
void print_move(Move);

typedef struct Moves {
  Move inner[MAX_MOVES_COUNT];
  Move* end;
} Moves;

#define MOVE_APPEND(moves, move) *(moves)->end++ = move
#define MOVE_INIT(moves)                                                       \
  memset(&moves, 0, sizeof(moves));                                            \
  moves.end = moves.inner
