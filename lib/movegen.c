#include "movegen.h"
#include "board.h"
#include "move.h"

uint8_t first_rank_attacks[64 * 8] = { 0 };

Bitboard g_pawn_attacks[COLOR_NB][SQ_NB];
Bitboard g_knight_attacks[SQ_NB];
Bitboard g_bishop_attacks[SQ_NB];
Bitboard g_rook_attacks[SQ_NB];

int g_bishop_relavent_bits[SQ_NB];
int g_rook_relavent_bits[SQ_NB];

Bitboard g_rook_magics[64] = {
  0x50800088104002a0ULL, 0x240004410002000ULL,  0x100150008200040ULL,
  0x8900200900100004ULL, 0x6600101448202200ULL, 0x8300010002080400ULL,
  0x1080808002000100ULL, 0x1e00048120420403ULL, 0x25800420844010ULL,
  0xc0400050002000ULL,   0x8802000801003ULL,    0x8088808010000800ULL,
  0x881000411000800ULL,  0x222000402001008ULL,  0x120808002000100ULL,
  0x6006000042108401ULL, 0x125800088c00aULL,    0x810004000402001ULL,
  0x1008110020010048ULL, 0x440120020420008ULL,  0x808008000400ULL,
  0x1000880140041020ULL, 0x200840002100108ULL,  0x20010640183ULL,
  0x3280228000c010ULL,   0x501200280400080ULL,  0x100100680200080ULL,
  0x4808001010020100ULL, 0x1040110100080004ULL, 0x4b000300240008ULL,
  0x223000101040200ULL,  0x108200011064ULL,     0x80002000400040ULL,
  0x4401020026004080ULL, 0x1100820012004020ULL, 0x801001001002008ULL,
  0x4420060602001020ULL, 0x1000401000208ULL,    0x1020804009001ULL,
  0x4082000104ULL,       0x8020209040008008ULL, 0x1a90084020064000ULL,
  0x1110001020008080ULL, 0x2004020120008ULL,    0x600c008008008004ULL,
  0x204000200048080ULL,  0x20110840008ULL,      0x8a20a08061020004ULL,
  0x8044344000800080ULL, 0x20004000300040ULL,   0x6002801000200280ULL,
  0x208080080100080ULL,  0x84800800040080ULL,   0x4800020080040080ULL,
  0x31000200440d00ULL,   0x9680800100004080ULL, 0x4000422013048001ULL,
  0xa008010650242ULL,    0x410010082001ULL,     0x1001000042109ULL,
  0x802010410200802ULL,  0x12000804100102ULL,   0xc80100891220804ULL,
  0x86008041002412ULL,
};

Bitboard g_bishop_magics[64] = {
  0x820c050e041300ULL,   0x8021408420014ULL,    0x210440080208000ULL,
  0x598048904291010ULL,  0x8002021000220000ULL, 0x81010840005044ULL,
  0x1004044402088060ULL, 0x4802802100500ULL,    0x80810440040ULL,
  0xc10012810a084ULL,    0x8840080828428810ULL, 0x404040410840c00ULL,
  0x3008011041084000ULL, 0x244402408001ULL,     0x8414c4060842c2ULL,
  0x900110401040202ULL,  0x12868810300080ULL,   0xd440202a8024400ULL,
  0x44a100c420040ULL,    0x38000401222580ULL,   0x262100c01200041ULL,
  0x2020001a0902800ULL,  0x8440082082040ULL,    0x4820801506411020ULL,
  0x488a0240100100ULL,   0x140104082028a208ULL, 0x18104018008420ULL,
  0x510040000401120ULL,  0x41001041004010ULL,   0x802020000209020ULL,
  0x2000810040841090ULL, 0x5c82002000940102ULL, 0x40080240000a080aULL,
  0x20c41000043002ULL,   0x9810104801440800ULL, 0x4a08022080200ULL,
  0x4400a0220120080ULL,  0x20e0080883044048ULL, 0x104080060208400ULL,
  0x820860886424c00ULL,  0x1082004001000ULL,    0xa000420821000404ULL,
  0x9008041005020ULL,    0x800902018001100ULL,  0x8040408201400ULL,
  0x220044481200a00ULL,  0x4020442400804041ULL, 0x1030010100300100ULL,
  0x20a5002080510ULL,    0x110420201200400ULL,  0x40004208908000ULL,
  0x400040002a080030ULL, 0x8020001002022000ULL, 0x20811020c5250a00ULL,
  0x240480200a20084ULL,  0x10090105260200ULL,   0x881040a440c4400ULL,
  0x200048404112500ULL,  0xd0081100411020ULL,   0x4132009010840444ULL,
  0x801000150da0208ULL,  0x22004110020a20ULL,   0xc00421024208ULL,
  0x420041022204110ULL,
};

Bitboard g_bishop[SQ_NB][512];
Bitboard g_rook[SQ_NB][4096];

Bitboard set_occupancy(int index, int nbits, Bitboard attacks) {
  Bitboard occ = EMPTY;
  for (int i = 0; i < nbits; ++i) {
    int sq = lsb(attacks);
    POP_BIT(attacks, sq);
    // pop_lsb(&attacks);
    if (index & (1 << i)) {
      occ |= (1ULL << sq);
    }
  }

  return occ;
}

Bitboard pawn_attacks_mask(Square sq, Color side) {
  Bitboard bb = EMPTY;
  SET_BIT(bb, sq);

  if (side == WHITE) {
    return ((bb << 7) & ~FILE_A | (bb << 9) & ~FILE_H);
  } else if (side == BLACK) {
    return ((bb >> 7) & ~FILE_H | (bb >> 9) & ~FILE_A);
  } else {
    assert(false && "invalid side");
  }
}

Bitboard knight_attacks_mask(Square sq) {
  Bitboard bb = EMPTY;
  Bitboard attacks = EMPTY;
  SET_BIT(bb, sq);
  attacks |= (bb << 0x11) & ~FILE_H;
  attacks |= (bb << 0x0A) & ~FILE_GH;
  attacks |= (bb >> 0x11) & ~FILE_A;
  attacks |= (bb >> 0x0A) & ~FILE_AB;
  attacks |= (bb << 0x0F) & ~FILE_A;
  attacks |= (bb << 0x06) & ~FILE_AB;
  attacks |= (bb >> 0x0F) & ~FILE_H;
  attacks |= (bb >> 0x06) & ~FILE_GH;
  return attacks;
}

Bitboard rank_attacks(Bitboard occ, Square sq) {
  uint8_t file = sq & 7;
  uint8_t rk = sq & 56; // rank * 8
  uint8_t rank_occ = (occ >> rk) & 2 * 63;
  Bitboard attacks = first_rank_attacks[4 * rank_occ + file];
  return attacks << rk;
}

Bitboard bishop_attacks_mask(Square sq) {
  Bitboard attacks = EMPTY;
  int tr = (int)sq / 8;
  int tf = (int)sq % 8;
  for (int r = tr + 1, f = tf + 1; r < 7 && f < 7; ++r, ++f) {
    attacks |= 1ULL << (r * 8 + f);
  }
  for (int r = tr - 1, f = tf + 1; r >= 1 && f < 7; --r, ++f) {
    attacks |= 1ULL << (r * 8 + f);
  }
  for (int r = tr + 1, f = tf - 1; r < 7 && f >= 1; ++r, --f) {
    attacks |= 1ULL << (r * 8 + f);
  }
  for (int r = tr - 1, f = tf - 1; r >= 1 && f >= 1; --r, --f) {
    attacks |= 1ULL << (r * 8 + f);
  }
  return attacks;
}

Bitboard rook_attacks_mask(Square sq) {
  Bitboard attacks = EMPTY;
  attacks |= (FILE_H << (sq & 7)) & ~(RANK_1 | RANK_8);
  attacks |= (RANK_1 << (sq & 56)) & ~(FILE_A | FILE_H);
  return attacks & ~(1ULL << sq);
}

// void pawn_moves(MoveList moves, Color color) {
//   const Bitboard empty = EMPTY_SQUARES;
//   if (color) {
//     Bitboard bb = piece[PC_B_PAWN] & RANK_7;
//     Bitboard single_push = bb >> 8;
//     Bitboard double_push = (single_push >> 8) & empty & RANK_5;
//     print_attack_bitboard(bb, single_push | double_push);
//   } else {
//     Bitboard bb = piece[PC_W_PAWN] & RANK_2;
//     Bitboard single_push = (bb << 8) & empty;
//     Bitboard double_push = (single_push << 8) & empty & RANK_4;
//     print_attack_bitboard(bb, single_push | double_push);
//   }
// }

Bitboard quiet_pawn_moves(Square sq, Color color) {
  Bitboard pawnbb = SQBB(sq);
  Bitboard movebb = 0ULL;
  if (color == WHITE) {
    Bitboard rank2pawn = pawnbb & RANK_2;
    movebb |= (pawnbb << 8) & EMPTY_SQUARES;
    if (movebb) {
      movebb |= (rank2pawn << 16) & EMPTY_SQUARES;
    }
  } else {
    Bitboard rank7pawn = pawnbb & RANK_7;
    movebb |= (pawnbb >> 8) & EMPTY_SQUARES;
    if (movebb) {
      movebb |= (rank7pawn >> 16) & EMPTY_SQUARES;
    }
  }

  return movebb;
}

void generate_moves(MoveList* moves) {
  for (int from = 0; from < 64; ++from) {
    Piece pc = board[from];
    if (PC_COLOR(pc) != side_to_move) {
      continue;
    }

    Bitboard bb = piece[pc];
    Bitboard b = EMPTY;
    if (pc == PC_W_PAWN) {
      b |= quiet_pawn_moves(from, WHITE);
    } else if (pc == PC_B_PAWN) {
      b |= quiet_pawn_moves(from, BLACK);
    }
    b |= attacks_bb(from);
    while (b) { MOVE_APPEND(moves, MOVE(from, pop_lsb(&b), 0)); }
  }
}

// void generate_moves(MoveList* moves) {
//   for (int p = NO_PIECE; p < PIECE_NB; ++p) {
//     Bitboard bb = piece[p];
//     if (side_to_move == WHITE) {
//       if (p == PC_W_PAWN) {
//         while (bb) {
//           int from = pop_lsb(&bb);
//           Bitboard pawnbb = 1ULL << from;
//           if ((pawnbb << 8) & EMPTY_SQUARES) {
//             MOVE_APPEND(moves, MOVE(from, from + 8, 0));
//             if ((pawnbb & RANK_2) && (pawnbb << 16) & EMPTY_SQUARES) {
//               MOVE_APPEND(moves, MOVE(from, from + 16, 0));
//             }
//           }
//         }
//       }
//     } else {
//       if (p == PC_B_PAWN) {
//         while (bb) {
//           int from = pop_lsb(&bb);
//           Bitboard pawnbb = 1ULL << from;
//           if ((pawnbb >> 8) & EMPTY_SQUARES) {
//             MOVE_APPEND(moves, MOVE(from, from - 8, 0));
//             if ((pawnbb & RANK_7) && (pawnbb >> 16) & EMPTY_SQUARES) {
//               MOVE_APPEND(moves, MOVE(from, from - 16, 0));
//             }
//           }
//         }
//       } else if (p == PC_B_KNIGHT) {
//         while (bb) {
//           int from = pop_lsb(&bb);
//           Bitboard b = knight_attacks(from) & ~US;
//           while (b) { MOVE_APPEND(moves, MOVE(from, pop_lsb(&b), 0)); }
//         }
//       } else if (p == PC_B_BISHOP) {
//         while (bb) {
//           int from = pop_lsb(&bb);
//           Bitboard b = bishop_attacks(from, OCCUPIED_SQUARES) & ~US;
//           while (b) { MOVE_APPEND(moves, MOVE(from, pop_lsb(&b), 0)); }
//         }
//       } else if (p == PC_B_ROOK) {
//         while (bb) {
//           int from = pop_lsb(&bb);
//           Bitboard b = rook_attacks(from, OCCUPIED_SQUARES) & ~US;
//           while (b) { MOVE_APPEND(moves, MOVE(from, pop_lsb(&b), 0)); }
//         }
//       } else if (p == PC_B_QUEEN) {
//         while (bb) {
//           int from = pop_lsb(&bb);
//           Bitboard b = queen_attacks(from, OCCUPIED_SQUARES) & ~US;
//           while (b) { MOVE_APPEND(moves, MOVE(from, pop_lsb(&b), 0)); }
//         }
//       }
//     }
//   }
// }

Bitboard attacks_bb(Square from) {
  if (COLOR_AT(from) != side_to_move) {
    return 0ULL;
  }
  switch (board[from]) {
    case PC_B_QUEEN:
    case PC_W_QUEEN: return queen_attacks(from, OCCUPIED_SQUARES) & ~US;
    case PC_B_ROOK:
    case PC_W_ROOK: return rook_attacks(from, OCCUPIED_SQUARES) & ~US;
    case PC_B_BISHOP:
    case PC_W_BISHOP: return bishop_attacks(from, OCCUPIED_SQUARES) & ~US;
    case PC_B_PAWN: return pawn_attacks(from, BLACK) & OPPONENT;
    case PC_W_PAWN: return pawn_attacks(from, WHITE) & OPPONENT;
    // case PC_B_PAWN: return quiet_pawn_moves(from, BLACK);
    // case PC_W_PAWN: return quiet_pawn_moves(from, WHITE);
    case PC_B_KNIGHT:
    case PC_W_KNIGHT: return knight_attacks(from) & ~US;
    case PC_B_KING:
    case PC_W_KING: return king_attacks(from) & ~US;
    case NO_PIECE: return 0;
    case PIECE_NB: return 0;
  }

  return 0;
}

Bitboard pawn_attacks(Square sq, Color side) {
  return g_pawn_attacks[side][sq];
}

Bitboard knight_attacks(Square sq) { return g_knight_attacks[sq]; }

Bitboard bishop_attacks(Square sq, Bitboard occ) {
  occ &= g_bishop_attacks[sq];
  occ *= g_bishop_magics[sq];
  occ >>= 64 - g_bishop_relavent_bits[sq];
  return g_bishop[sq][occ];
}

Bitboard rook_attacks(Square sq, Bitboard occ) {
  occ &= g_rook_attacks[sq];
  occ *= g_rook_magics[sq];
  occ >>= 64 - g_rook_relavent_bits[sq];
  return g_rook[sq][occ];
}

Bitboard queen_attacks(Square sq, Bitboard occ) {
  return bishop_attacks(sq, occ) | rook_attacks(sq, occ);
}

Bitboard king_attacks(Square sq) {
  Bitboard bb = SQBB(sq);
  Bitboard attacks = EMPTY;
  attacks |= (bb << 1) & ~FILE_H | (bb >> 1) & ~FILE_A | (bb << 8) | (bb >> 8);
  attacks |= g_pawn_attacks[WHITE][sq];
  attacks |= g_pawn_attacks[BLACK][sq];
  return attacks;
}

Bitboard bishop_attacks_on_the_fly(Square sq, Bitboard occ) {
  Bitboard attacks = EMPTY;
  int tr = (int)sq / 8;
  int tf = (int)sq % 8;
  for (int r = tr + 1, f = tf + 1; r < 8 && f < 8; ++r, ++f) {
    attacks |= 1ULL << (r * 8 + f);
    if ((1ULL << (r * 8 + f)) & occ) {
      break;
    }
  }
  for (int r = tr - 1, f = tf + 1; r >= 0 && f < 8; --r, ++f) {
    attacks |= 1ULL << (r * 8 + f);
    if ((1ULL << (r * 8 + f)) & occ) {
      break;
    }
  }
  for (int r = tr + 1, f = tf - 1; r < 8 && f >= 0; ++r, --f) {
    attacks |= 1ULL << (r * 8 + f);
    if ((1ULL << (r * 8 + f)) & occ) {
      break;
    }
  }
  for (int r = tr - 1, f = tf - 1; r >= 0 && f >= 0; --r, --f) {
    attacks |= 1ULL << (r * 8 + f);
    if ((1ULL << (r * 8 + f)) & occ) {
      break;
    }
  }
  return attacks;
}

Bitboard rook_attacks_on_the_fly(Square sq, Bitboard occ) {
  Bitboard attacks = EMPTY;
  int tr = (int)sq / 8;
  int tf = (int)sq % 8;
  for (int r = tr + 1; r <= 7; ++r) {
    attacks |= 1ULL << (r * 8 + tf);
    if ((1ULL << (r * 8 + tf)) & occ) {
      break;
    }
  }

  for (int r = tr - 1; r >= 0; r--) {
    attacks |= (1ULL << (r * 8 + tf));
    if ((1ULL << (r * 8 + tf)) & occ) {
      break;
    }
  }

  for (int f = tf + 1; f <= 7; f++) {
    attacks |= (1ULL << (tr * 8 + f));
    if ((1ULL << (tr * 8 + f)) & occ) {
      break;
    }
  }

  for (int f = tf - 1; f >= 0; f--) {
    attacks |= (1ULL << (tr * 8 + f));
    if ((1ULL << (tr * 8 + f)) & occ) {
      break;
    }
  }

  return attacks;
}

void init_leaper_attacks() {
  for (int sq = SQ_ZERO; sq < SQ_NB; ++sq) {
    // Pawn attacks
    g_pawn_attacks[WHITE][sq] = pawn_attacks_mask(sq, WHITE);
    g_pawn_attacks[BLACK][sq] = pawn_attacks_mask(sq, BLACK);

    // Knight attacks
    g_knight_attacks[sq] = knight_attacks_mask(sq);
  }
}

void init_slider_attacks() {
  for (int sq = SQ_ZERO; sq < SQ_NB; ++sq) {
    Bitboard mask = EMPTY;
    // Bishop attacks
    mask = bishop_attacks_mask(sq);
    g_bishop_attacks[sq] = mask;
    int relavent_bits_count = count_bits(mask);
    g_bishop_relavent_bits[sq] = relavent_bits_count;
    // g_bishop_magics[sq] = find_magic_number(sq, relavent_bits_count, true);
    int occ_indices = (1 << relavent_bits_count);
    for (int i = 0; i < occ_indices; ++i) {
      Bitboard occ = set_occupancy(i, relavent_bits_count, mask);
      int magic_idx = (int)((occ * g_bishop_magics[sq]) >>
                            (64 - g_bishop_relavent_bits[sq]));
      g_bishop[sq][magic_idx] = bishop_attacks_on_the_fly(sq, occ);
    }

    // rook attacks
    mask = rook_attacks_mask(sq);
    g_rook_attacks[sq] = mask;
    relavent_bits_count = count_bits(mask);
    g_rook_relavent_bits[sq] = relavent_bits_count;
    // g_rook_magics[sq] = find_magic_number(sq, relavent_bits_count, false);
    occ_indices = (1 << relavent_bits_count);
    for (int i = 0; i < occ_indices; ++i) {
      Bitboard occ = set_occupancy(i, relavent_bits_count, mask);
      int magic_idx =
          (int)((occ * g_rook_magics[sq]) >> (64 - g_rook_relavent_bits[sq]));
      g_rook[sq][magic_idx] = rook_attacks_on_the_fly(sq, occ);
    }
  }
}

void print_moves() {
  MoveList moves = MAKE_MOVE_LIST();
  generate_moves(&moves);
  printf("moves: %zu\n", moves.length);
  for (size_t i = 0; i < moves.length; ++i) { print_move(moves.inner[i]); }
  free(moves.inner);
}
