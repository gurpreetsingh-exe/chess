#include "bitboard.h"
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