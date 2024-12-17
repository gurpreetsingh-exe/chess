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

#define FORCE_INLINE __attribute__((always_inline))
