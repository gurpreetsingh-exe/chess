// clang-format off
#include <GL/glew.h>
// clang-format on

#include "stb_image.h"

#include "bitboard.h"
#include "board.h"
#include "engine.h"
#include "move.h"
#include "movegen.h"
#include "types.h"

#include <GLFW/glfw3.h>

#define UPLOAD_PIECE(pc, i)                                                    \
  {                                                                            \
    glActiveTexture(GL_TEXTURE0 + i);                                          \
    glBindTexture(GL_TEXTURE_2D, gl_piece_textures[i]);                        \
    char name[16];                                                             \
    memset(name, 0, sizeof(name));                                             \
    snprintf(name, sizeof(name), "u_p%d", i);                                  \
    GLint loc = glGetUniformLocation(program, name);                           \
    glUniform1i(loc, i);                                                       \
  }

// clang-format off
const char* piece_textures[] = {
  "",
  "images/wP.png",
  "images/wN.png",
  "images/wB.png",
  "images/wR.png",
  "images/wQ.png",
  "images/wK.png",
  "",
  "",
  "images/bP.png",
  "images/bN.png",
  "images/bB.png",
  "images/bR.png",
  "images/bQ.png",
  "images/bK.png",
};
// clang-format on

GLFWwindow* window;
bool quit = false;
GLuint program;
GLuint gl_piece_textures[ARRLEN(piece_textures)];
int attacks[SQ_NB];
Piece grabbed_piece = NO_PIECE;
int active = -1;
Piece gboard[SQ_NB];
Move move;
Bitboard legal_moves = EMPTY;

GLuint create_shader(GLuint type, const char* source) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, 0);
  glCompileShader(shader);

  int result;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    int length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    char* info_log = malloc(sizeof(char) * (length + 1));
    glGetShaderInfoLog(shader, length, &length, &info_log[0]);
    glDeleteShader(shader);
    info_log[length] = 0;
    fprintf(stderr, "%s\n", info_log);
  }

  return shader;
}

void setup_shader(char* vertex_source, char* fragment_source) {
  program = glCreateProgram();
  GLuint vert = create_shader(GL_VERTEX_SHADER, vertex_source);
  GLuint frag = create_shader(GL_FRAGMENT_SHADER, fragment_source);
  glAttachShader(program, vert);
  glAttachShader(program, frag);
  glLinkProgram(program);
  int is_linked;
  glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
  if (is_linked == GL_FALSE) {
    int length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    char* info_log = malloc(sizeof(char) * (length + 1));
    glGetProgramInfoLog(program, length, &length, &info_log[0]);
    glDeleteProgram(program);
    info_log[length] = 0;
    fprintf(stderr, "%s\n", info_log);
  }
  glDetachShader(program, vert);
  glDetachShader(program, frag);
  free(vertex_source);
  free(fragment_source);
}

GLuint load_texture(const char* path) {
  int w = 0;
  int h = 0;
  int channels = 0;
  stbi_set_flip_vertically_on_load(true);
  void* data = stbi_loadf(path, &w, &h, &channels, 0);
  GLenum internal_format = GL_NONE;
  switch (channels) {
    case 3: {
      internal_format = GL_RGB32F;
    } break;
    case 4: {
      internal_format = GL_RGBA32F;
    } break;
    default: {
      assert(false);
    }
  }

  if (internal_format == GL_NONE) {
    assert(false);
  }

  GLuint id;
  glCreateTextures(GL_TEXTURE_2D, 1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  GLenum format = GL_NONE;
  switch (internal_format) {
    case GL_RGB8:
    case GL_RGB32F: {
      format = GL_RGB;
    } break;
    case GL_RGBA32F: {
      format = GL_RGBA;
    } break;
    default: assert(false && "format not found");
  }

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, internal_format, w, h, 0, format, GL_FLOAT,
               data);

  stbi_image_free(data);
  glBindTexture(GL_TEXTURE_2D, 0);
  return id;
}

void load_textures() {
  for (size_t i = 0; i < ARRLEN(piece_textures); ++i) {
    if (!piece_textures[i][0]) {
      continue;
    }
    gl_piece_textures[i] = load_texture(piece_textures[i]);
  }
}

char* read_file(const char* path) {
  FILE* f = fopen(path, "r");

  if (!f) {
    fprintf(stderr, "cannot open \"%s\"\n  %s\n", path, strerror(errno));
  }

  if (fseek(f, 0L, SEEK_END)) {
    fprintf(stderr, "%s\n", strerror(errno));
  }

  long file_size = ftell(f);
  if (file_size < 0) {
    fprintf(stderr, "cannot get file size\n  %s\n", strerror(errno));
  }

  rewind(f);

  size_t size = (size_t)file_size;
  char* content = malloc(sizeof(char) * (size + 1));
  size_t read_bytes = fread(content, 1, size, f);
  content[size] = 0;

  if (read_bytes != size) {
    int e = feof(f);
    if (e) {
      fprintf(stderr, "unexpected end of file\n");
    } else {
      e = ferror(f);
      fprintf(stderr, "cannot read file\n  %s\n", strerror(e));
    }
  }

  fclose(f);
  return content;
}

int get_square() {
  double x, y;
  glfwGetCursorPos(window, &x, &y);
  y = (640.0 - y) / 640.0 * 8.0;
  x = x / 640.0 * 8.0;
  return (int)x + (int)y * 8;
}

void print_square(int sq) { printf("%c%d\n", 'a' + (sq % 8), (sq / 8) + 1); }

void highlight_squares(Bitboard bb) {
  memset(attacks, 0, sizeof(attacks));
  while (bb) { attacks[pop_lsb(&bb)] = 1; }
  glUniform1iv(glGetUniformLocation(program, "u_attacks"), 64, attacks);
}

void update_attacks() {
  highlight_squares(active != -1 ? get_moves(active) : 0ULL);
}

Piece gtake_piece(Square sq) {
  Piece pc = gboard[sq];
  gboard[sq] = NO_PIECE;
  return pc;
}

void gput_piece(Piece pc, Square sq) { gboard[sq] = pc; }

void mouse_button_callback(GLFWwindow* win, int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    if (action == GLFW_PRESS) {
      if (active == get_square()) {
        grabbed_piece = gtake_piece(active);
        move = MOVE(active, 0, 0);
        return;
      }
      active = get_square();
      glUniform1i(glGetUniformLocation(program, "u_active"), active);
      update_attacks();
      grabbed_piece = gtake_piece(active);
      move = MOVE(active, 0, 0);
    } else if (action == GLFW_RELEASE) {
      if (grabbed_piece == NO_PIECE) {
        return;
      }
      int from = MOVE_FROM(move);
      int to = get_square();
      if (!(get_moves(from) & SQBB(to))) {
        gput_piece(grabbed_piece, from);
        grabbed_piece = 0;
        return;
      }
      int move_type = NORMAL;
      if (PC_TYPE(board[from]) == PTY_KING) {
        if (abs(from - to) == 2) {
          move_type = CASTLING;
        }
      } else if (PC_TYPE(board[from]) == PTY_PAWN) {
        if (board[to] == NO_PIECE) {
          if (abs(to - from) == 9 || abs(to - from) == 7) {
            move_type = EN_PASSANT;
          }
        }
      }
      gput_piece(grabbed_piece, to);
      Move m = MOVE(from, to, move_type);
      make_move(m);
      // TODO: maybe precalculate all the legal moves for the next turn
      // here
      memcpy(gboard, board, sizeof(board));
      grabbed_piece = 0;
      if (active == to) {
        return;
      }
      active = -1;
      update_attacks();
    }
  }
}

void key_callback(GLFWwindow* win, int key, int scancode, int action,
                  int mods) {
  if (action != GLFW_PRESS) {
    return;
  }

  switch (key) {
    case GLFW_KEY_ESCAPE:
    case GLFW_KEY_Q: {
      quit = true;
    } break;
    case GLFW_KEY_M: {
      print_moves();
    } break;
    case GLFW_KEY_O: {
      highlight_squares(OPPONENT);
    } break;
    case GLFW_KEY_RIGHT: {
      if (!(*prev_moves.end)) {
        return;
      }
      make_move(*prev_moves.end);
      memcpy(gboard, board, sizeof(board));
    } break;
    case GLFW_KEY_LEFT: {
      if (prev_moves.inner >= prev_moves.end) {
        return;
      }
      Move m = *(prev_moves.end - 1);
      if (!m) {
        return;
      }
      unmake_move(m);
      memcpy(gboard, board, sizeof(board));
    } break;
    case GLFW_KEY_B: {
      highlight_squares(pins[BLACK] | pins[WHITE]);
    } break;
    case GLFW_KEY_P: {
      highlight_squares(pinners[BLACK] | pinners[WHITE]);
    } break;
  }
}

int main() {
  if (!glfwInit()) {
    return -1;
  }

  window = glfwCreateWindow(640, 640, "Chess", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetKeyCallback(window, key_callback);

  if (glewInit() == GLEW_OK) {
    printf("GL version: %s\n", (const char*)glGetString(GL_VERSION));
  }

  engine_start();
  // reset_position();
  // set_position("4r2k/8/8/8/4B3/8/8/4K3 w - -");
  memcpy(gboard, board, sizeof(board));
  char* vertex_source = read_file("shader/vertex.glsl");
  char* fragment_source = read_file("shader/fragment.glsl");
  setup_shader(vertex_source, fragment_source);
  load_textures();

  glUseProgram(program);
  UPLOAD_PIECE(PC_W_PAWN, 1);
  UPLOAD_PIECE(PC_W_KNIGHT, 2);
  UPLOAD_PIECE(PC_W_BISHOP, 3);
  UPLOAD_PIECE(PC_W_ROOK, 4);
  UPLOAD_PIECE(PC_W_QUEEN, 5);
  UPLOAD_PIECE(PC_W_KING, 6);
  UPLOAD_PIECE(PC_B_PAWN, 9);
  UPLOAD_PIECE(PC_B_KNIGHT, 10);
  UPLOAD_PIECE(PC_B_BISHOP, 11);
  UPLOAD_PIECE(PC_B_ROOK, 12);
  UPLOAD_PIECE(PC_B_QUEEN, 13);
  UPLOAD_PIECE(PC_B_KING, 14);

  while (!glfwWindowShouldClose(window) && !quit) {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);

    glUniform1iv(glGetUniformLocation(program, "u_board"), 64, gboard);
    glUniform1i(glGetUniformLocation(program, "u_grabbed_piece"),
                grabbed_piece);
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    glUniform2f(glGetUniformLocation(program, "u_mouse"), x, y);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
