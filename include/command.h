#pragma once

typedef enum CommandKind {
  CMD_QUIT,
  CMD_PRINT_BOARD,
  CMD_SET_POSITION,
} CommandKind;

typedef struct Command {
  CommandKind kind;
} Command;

Command parse_command(char*);
