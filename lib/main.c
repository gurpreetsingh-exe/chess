#include "board.h"
#include "movegen.h"
#include "repl.h"
#include "types.h"

Bitboard diag_mask(int sq) {
  const Bitboard t = 0x8040201008040201ULL;
  int diag = (sq & 7) - (sq >> 3);
  return diag >= 0 ? t >> diag * 8 : t << -diag * 8;
}

Bitboard anti_diag_mask(int sq) {
  const Bitboard t = 0x0102040810204080ULL;
  int diag = 7 - (sq & 7) - (sq >> 3);
  return diag >= 0 ? t >> diag * 8 : t << -diag * 8;
}

Bitboard rank_mask(int sq) { return 0xffULL << (sq & 56); }

Bitboard file_mask(int sq) { return 0x0101010101010101ULL << (sq & 7); }

size_t perft(int depth) {
  Moves moves;
  MOVE_INIT(moves);
  size_t nodes = 0;

  if (depth == 0) {
    return 1;
  }

  generate_moves(&moves);
  for (size_t i = 0; i < moves.end - moves.inner; i++) {
    Move move = moves.inner[i];
    make_move(move);
    nodes += perft(depth - 1);
    unmake_move(move);
  }
  return nodes;
}

void print_perft(int depth) {
  printf("perft(%d) = %zu\n", depth, perft(depth));
}

int engine_start() {
  TIME("init_leaper_attacks", init_leaper_attacks());
  TIME("init_slider_attacks", init_slider_attacks());
  setup_starting_position();

  // 0     1
  // 1     20
  // 2     400
  // 3     8,902
  // 4     197,281
  // 5     4,865,609
  // 6     119,060,324
  // 7     3,195,901,860
  // 8     84,998,978,956
  // 9     2,439,530,234,167
  // 10    69,352,859,712,417
  // 11    2,097,651,003,696,806
  // 12    62,854,969,236,701,747
  // 13    1,981,066,775,000,396,239
  // 14    61,885,021,521,585,529,237
  // 15    2,015,099,950,053,364,471,960

  print_perft(5);
  // for (int i = 0; i < 8; ++i) {
  //   TIME("perft", { print_perft(i); });
  // }

  // print_attack_bitboard(bb, pawn_attacks(bb, WHITE));
  // SET_BIT(bb, a4);
  // SET_BIT(bb, a1);
  // SET_BIT(bb, a8);
  // SET_BIT(bb, h1);
  // SET_BIT(bb, h8);

  // Bitboard bb = EMPTY;
  // Bitboard occ = EMPTY;
  // int sq = e4;
  // SET_BIT(bb, sq);
  // SET_BIT(occ, f5);
  // SET_BIT(occ, e7);
  // SET_BIT(occ, c4);

  // print_attack_bitboard(anti_diag_mask(a4), EMPTY);
  // print_attack_bitboard(by_type_bb[ALL_PIECES],
  //                       rook_attacks(h8, by_type_bb[ALL_PIECES]));

  // for (int i = 0; i < 64; ++i) {
  //   Bitboard bb = EMPTY;
  //   SET_BIT(bb, i);
  //   print_attack_bitboard(bb, rook_attacks(i));
  // }

  // int sq = f1;
  // print_attack_bitboard(bb, knight_attacks(a1) | knight_attacks(a8) |
  //                               knight_attacks(h1) | knight_attacks(h8));
  // print_bitboard(bb);
  // int sq = e3;
  // print_attack_bitboard(1ULL << sq,
  //                       singlePushTargets(1ULL << sq, ~(1ULL << sq), WHITE));

  // start_repl();
  // print_board();
  // print_moves();
  return 0;
}
