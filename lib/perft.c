#include "perft.h"
#include "board.h"
#include "movegen.h"

// #define BULK_COUNTING

size_t perft(int depth) {
#ifndef BULK_COUNTING
  if (depth == 0) {
    return 1;
  }
#endif

  size_t nodes = 0;
  Moves moves;
  MOVE_INIT(moves);
  generate_moves(&moves);

#ifdef BULK_COUNTING
  if (depth == 1) {
    return (size_t)(moves.end - moves.inner);
  }
#endif

  for (size_t i = 0; i < moves.end - moves.inner; i++) {
    Move move = moves.inner[i];
    make_move(move);
    nodes += perft(depth - 1);
    unmake_move(move);
  }
  return nodes;
}

size_t perft_driver(int depth) {
  if (depth == 0) {
    return 1;
  }

  size_t nodes = 0;
  Moves moves;
  MOVE_INIT(moves);
  generate_moves(&moves);

  for (size_t i = 0; i < moves.end - moves.inner; i++) {
    Move move = moves.inner[i];
    make_move(move);
    size_t n = perft(depth - 1);
    nodes += n;
    string_of_move(move);
    printf(": %zu\n", n);
    unmake_move(move);
  }
  return nodes;
}
