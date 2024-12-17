#include "move.h"
#include "types.h"

void print_move(Move move) {
  printf("%s%s\n", square_to_string[MOVE_FROM(move)],
         square_to_string[MOVE_TO(move)]);
}

void ensure_capacity(MoveList* moves) {
  if (moves->capacity <= moves->length) {
    moves->capacity *= 2;
    moves->inner = realloc(moves->inner, sizeof(Move) * moves->capacity);
  }
}
