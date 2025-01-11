#include "board.h"
#include "movegen.h"
#include "types.h"

int engine_start() {
  TIME("init_leaper_attacks", init_leaper_attacks());
  TIME("init_slider_attacks", init_slider_attacks());
  setup_starting_position();
  return 0;
}
