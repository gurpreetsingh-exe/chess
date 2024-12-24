#pragma once

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct string {
  char* ptr;
  size_t size;
} string;

typedef struct string_view {
  const char* ptr;
  size_t size;
} string_view;

#define FORCE_INLINE inline __attribute__((always_inline))

#define PRAGMA(S) _Pragma(#S)
#define PUSH_IGNORE_WARNING(W)                                                 \
  PRAGMA(GCC diagnostic push) PRAGMA(GCC diagnostic ignored W)
#define POP_IGNORE_WARNING() PRAGMA(GCC diagnostic pop)

#if NDEBUG
#define ASAN_BREAKPOINT(c)
#else
#define ASAN_BREAKPOINT(c)                                                     \
  if (!c) {                                                                    \
    int a##__COUNTER__ = *(int*)0;                                             \
  }
#endif
