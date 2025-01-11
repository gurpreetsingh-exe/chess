#include "board.h"
#include "engine.h"
#include "perft.h"
#include "types.h"

void print_perft(int depth) {
  size_t n = perft(depth);
  printf("perft(%d) = %8zu | %6zu | %6zu | %6zu\n", depth, n,
         move_count.captures, move_count.epassant, move_count.castles);
}

bool test_perft(int max_depth, size_t* nodes) {
  for (int depth = 0; depth < max_depth; ++depth) {
    size_t computed = perft(depth);
    if (computed != nodes[depth]) {
      fprintf(stderr,
              "unexpected nodes count at depth %d\n    expected: %zu\n    "
              "found: %zu\n",
              depth, nodes[depth], computed);
      return false;
    }
  }

  return true;
}

#define TEST(name, ...)                                                        \
  do {                                                                         \
    size_t __nodes[] = { __VA_ARGS__ };                                        \
    if (!test_perft(ARRLEN(__nodes), __nodes)) {                               \
      fprintf(stderr, "%s FAILED\n", name);                                    \
    }                                                                          \
  } while (0)

int main() {
  engine_start();
  set_position("rnbqkbnr/8/8/8/8/8/8/RNBQKBNR w KQkq - ");
  // TEST("no pawns position", 1, 50, 2125, 96062, 4200525, 191462298,
  // 8509434052);
  TEST("no pawns position", 1, 50, 2125, 96062, 4200525, 191462298);

  // TEST("starting position", 1, 20, 400, 8902, 197281, 4865609, 119060324);
  // set_position(
  //     "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ");
  // // print_perft(4);
  // perft_driver(4);

  // TEST("kiwipete", 1, 48, 2039, 97862, 4085603, 193690690, 8031647685);
}
