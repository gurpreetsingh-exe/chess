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

int engine_start() {
  TIME("init_leaper_attacks", init_leaper_attacks());
  TIME("init_slider_attacks", init_slider_attacks());
  setup_starting_position();

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
