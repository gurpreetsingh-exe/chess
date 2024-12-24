#include "bitboard.h"
#include "board.h"
#include "movegen.h"
#include "types.h"

// #define FANCY

struct {
  const char* hbar;
  const char* vbar;
  const char* topright;
  const char* topleft;
  const char* bottomright;
  const char* bottomleft;
} chars = {
#if defined(FANCY)
  .hbar = "│",
  .vbar = "─",
  .topright = "╮",
  .topleft = "╭",
  .bottomright = "╯",
  .bottomleft = "╰"
#else
  .hbar = " ",
  .vbar = "",
  .topright = "",
  .topleft = "",
  .bottomright = "",
  .bottomleft = ""
#endif
};

void print_bitboard(Bitboard bb) {
  printf("    %s", chars.topleft);
  for (int i = 0; i < 17; ++i) { printf("%s", chars.vbar); }
  printf("%s\n", chars.topright);
  for (int j = GRID_SIZE - 1; j >= 0; --j) {
    printf("  %d %s ", j + 1, chars.hbar);
    for (int i = 0; i < GRID_SIZE; ++i) {
      int idx = i + j * GRID_SIZE;
      printf("%s ", GET_BIT(bb, idx) ? "◘" : "·");
      // printf("%s ", GET_BIT(bb, idx) ? "■" : "◘");
      // printf("%s ", GET_BIT(bb, idx) ? "▢" : "·");
    }
    printf("%s\n", chars.hbar);
  }
  printf("    %s", chars.bottomleft);
  for (int i = 0; i < 17; ++i) { printf("%s", chars.vbar); }
  printf("%s\n", chars.bottomright);
  printf("      a b c d e f g h\n\n");
}

FORCE_INLINE Bitboard between_bb(Square sq1, Square sq2) {
  const Bitboard m1 = -1ULL;
  const Bitboard a2a7 = 0x0001010101010100ULL;
  const Bitboard b2g7 = 0x0040201008040200ULL;
  const Bitboard h1b7 = 0x0002040810204080ULL;
  Bitboard btwn, line, rank, file;

  btwn = (m1 << sq1) ^ (m1 << sq2);
  file = (sq2 & 7) - (sq1 & 7);
  rank = ((sq2 | 7) - sq1) >> 3;
  line = ((file & 7) - 1) & a2a7;
  line += 2 * (((rank & 7) - 1) >> 58);
  line += (((rank - file) & 15) - 1) & b2g7;
  line += (((rank + file) & 15) - 1) & h1b7;
  line *= btwn & -btwn;
  return line & btwn;
}

Bitboard attackers_to(Square sq, Bitboard occ) {
  return (pawn_attacks(sq, BLACK) & piece[PC_W_PAWN]) |
         (pawn_attacks(sq, WHITE) & piece[PC_B_PAWN]) |
         (knight_attacks(sq) & type[PTY_KNIGHT]) |
         (rook_attacks(sq, occ) & (type[PTY_ROOK] | type[PTY_QUEEN])) |
         (bishop_attacks(sq, occ) & (type[PTY_BISHOP] | type[PTY_QUEEN])) |
         (king_attacks(sq) & type[PTY_KING]);
}

FORCE_INLINE int count_bits(Bitboard bb) {
  int count = 0;
  while (bb) {
    count++;
    bb &= bb - 1;
  }
  return count;
}

FORCE_INLINE int lsb(Bitboard bb) {
  ASAN_BREAKPOINT(bb);
  assert(bb);
#if defined(__GNUC__)
  return __builtin_ctzll(bb);
#elif defined(_MSC_VER)
  unsigned long idx;
  _BitScanForward64(&idx, b);
  return idx;
#else
  return count_bits((bb & -bb) - 1);
#endif
}

int pop_lsb(Bitboard* bb) {
  ASAN_BREAKPOINT(*bb);
  assert(*bb);
  const int s = lsb(*bb);
  *bb &= *bb - 1;
  return s;
}

void print_attack_bitboard(Bitboard bb, Bitboard attack_bb) {
  for (int j = GRID_SIZE - 1; j >= 0; --j) {
    printf("  %d   ", j + 1);
    for (int i = 0; i < GRID_SIZE; ++i) {
      int idx = i + j * GRID_SIZE;
      bool is_attack = GET_BIT(attack_bb, idx);
      bool is_occupied = GET_BIT(bb, idx);
      printf("%s", is_attack     ? "\x1b[31;1m"
                   : is_occupied ? "\x1b[1m"
                                 : "\x1b[90;1;2m");
      printf("%s ", is_occupied ? "◘" : "·");
      printf("\x1b[0m");
    }
    printf("\n");
  }
  printf("\n      a b c d e f g h\n\n");
}

Bitboard find_magic_number(int sq, int relavent_bits, bool bishop) {
  Bitboard occ[4096];
  Bitboard attacks[4096];
  Bitboard used_attacks[4096];
  Bitboard attack_mask =
      bishop ? bishop_attacks_mask(sq) : rook_attacks_mask(sq);
  int occ_indices = 1 << relavent_bits;
  for (int i = 0; i < occ_indices; ++i) {
    occ[i] = set_occupancy(i, relavent_bits, attack_mask);
    attacks[i] = bishop ? bishop_attacks_on_the_fly(sq, occ[i])
                        : rook_attacks_on_the_fly(sq, occ[i]);
  }

  for (int i = 0; i < 100000000; ++i) {
    uint64_t magic = get_random() & get_random() & get_random();
    if (count_bits((attack_mask * magic) & 0xff00000000000000) < 6) {
      continue;
    }

    memset(used_attacks, EMPTY, sizeof(used_attacks));
    int idx, fail;
    for (idx = 0, fail = 0; !fail && idx < occ_indices; ++idx) {
      int magic_idx = (int)((occ[idx] * magic) >> (64 - relavent_bits));
      if (used_attacks[magic_idx] == EMPTY) {
        used_attacks[magic_idx] = attacks[idx];
      } else if (used_attacks[magic_idx] != attacks[idx]) {
        fail = 1;
      }
    }
    if (!fail) {
      return magic;
    }
  }

  assert(false && "magic number failed");
  return EMPTY;
}
