#include "repl.h"
#include "board.h"
#include "movegen.h"

static const char* help_text = "    b, board          print the board\n"
                               "    q, quit           quit the engine\n"
                               "    h, help           print this help message\n"
                               "\n";

static void print_help() { printf("%s", help_text); }

static void parse_input(char* input) {
  while (*input) {
    const char* start = input;
    if (isalpha(*input)) {
      while (isalnum(*input)) { input++; }
      char buf[64] = {};
      memcpy(buf, start, (size_t)(input - start));
      printf("%s", buf);
    }
    ++input;
  }
}

void start_repl() {
  char input[MAX_INPUT_SIZE] = { 0 };
  while (true) {
    printf("> ");
    memset(input, 0, MAX_INPUT_SIZE);
    if (fgets(input, MAX_INPUT_SIZE, stdin) == nullptr) {
      if (ferror(stdin)) {
        fprintf(stderr, "error!\n");
        exit(1);
      }
      printf("\n");
      break;
    }

    char* end = strchr(input, '\n');
    if (end == NULL) {
      if (!feof(stdin)) {
        fprintf(stderr, "error: line too long for buffer!\n");
        exit(1);
      }
    } else {
      *end = '\0';
    }

    if (STREQ(input, "q") || STREQ(input, "quit")) {
      break;
    } else if (STREQ(input, "b") || STREQ(input, "board")) {
      print_board();
    } else if (STREQ(input, "m") || STREQ(input, "moves")) {
      print_moves();
    } else if (STREQ(input, "h") || STREQ(input, "help")) {
      print_help();
    } else {
      parse_input(input);
    }

    // printf("%s%c", input, end - input == 0 ? '\0' : '\n');
  }
}
