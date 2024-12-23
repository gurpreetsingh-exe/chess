#include "move.h"

void string_of_move(Move move) {
  printf("%s%s", square_to_string[MOVE_FROM(move)],
         square_to_string[MOVE_TO(move)]);
}

void print_move(Move move) {
  printf("%s%s\n", square_to_string[MOVE_FROM(move)],
         square_to_string[MOVE_TO(move)]);
}
