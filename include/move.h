#pragma once

#define MOVE(from, to, flags)                                                  \
  (((flags)&0xf) << 12) | (((from)&0x3f) << 6) | ((to)&0x3f)
#define MOVE_TO(move) ((move)&0x3f)
#define MOVE_FROM(move) (((move) >> 6) & 0x3f)
#define MOVE_FLAGS(move) (((move) >> 12) & 0x3f)

typedef uint16_t Move;
void print_move(Move);

#define MOVES_DEFAULT_CAPACITY 4096

typedef struct MoveList {
  Move* inner;
  size_t length;
  size_t capacity;
} MoveList;

void ensure_capacity(MoveList*);

#define MAKE_MOVE_LIST()                                                       \
  (MoveList) {                                                                 \
    .inner = malloc(sizeof(Move) * MOVES_DEFAULT_CAPACITY), .length = 0,       \
    .capacity = MOVES_DEFAULT_CAPACITY                                         \
  }

#define MOVE_APPEND(moves, move)                                               \
  do {                                                                         \
    ensure_capacity(moves);                                                    \
    moves->inner[moves->length++] = move;                                      \
  } while (0)
