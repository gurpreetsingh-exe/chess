#include "board.h"
#include "engine.h"
#include "perft.h"
#include "repl.h"
#include "types.h"

int main(int argc, char** argv) {
  engine_start();
  assert(argc == 3);
  set_position(argv[1]);
  perft_driver(atoi(argv[2]));
}
