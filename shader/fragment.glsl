#version 460 core

#extension GL_ARB_gpu_shader_int64 : require

#define GAMMA_CORRECTION
#define PIECES                                                                 \
  X(W_P, 1)                                                                    \
  X(W_N, 2)                                                                    \
  X(W_B, 3)                                                                    \
  X(W_R, 4)                                                                    \
  X(W_Q, 5)                                                                    \
  X(W_K, 6)                                                                    \
  X(B_P, 9)                                                                    \
  X(B_N, 10)                                                                   \
  X(B_B, 11)                                                                   \
  X(B_R, 12)                                                                   \
  X(B_Q, 13)                                                                   \
  X(B_K, 14)

in vec2 uv;
out vec4 color;

uniform int u_board[64];
uniform int u_attacks[64];
uniform int u_active;

#define X(pc, i) const int pc = i;
PIECES
#undef X

#define X(_, i) uniform sampler2D u_p##i;
PIECES
#undef X

const vec3 dark = vec3(154, 68, 29) / 255.f;
const vec3 light = vec3(243, 188, 157) / 255.f;

vec4 piece_by_id(int piece_id, vec2 uv) {
  switch (piece_id) {
    case 0:
      return vec4(1.0f, 1.0f, 1.0f, 0.0f);
#define X(_, i)                                                                \
  case i:                                                                      \
    return texture2D(u_p##i, uv);
      PIECES
#undef X
    default:
      return vec4(1.0f, 0.0f, 1.0f, 1.0f);
  }
  return vec4(1.0f, 0.0f, 1.0f, 1.0f);
}

vec3 grid(vec2 uv) {
  uv *= 8;
  vec2 grid = floor(uv);
  int c = int(grid.x + grid.y) % 2;
  vec2 iuv = uv - grid;
  return vec3(iuv, c);
}

void main() {
    vec2 uv0 = uv * 8.;
    vec2 uvf = floor(uv0);
    int square = int(dot(uvf, vec2(1, 8)));
    vec3 g = grid(uv);
    vec3 bg = mix(light, dark, g.z);
    bg = mix(bg, vec3(.85, .15, .15) * (g.z + 0.75), float(u_attacks[square]));
    bg = mix(bg, vec3(.85, .85, .15), float(square == u_active));
    vec3 c = bg;
    vec4 pc = piece_by_id(u_board[square], g.xy);
    c = mix(bg, pc.rgb, pc.a);
    const float gamma = 2.2;
#if defined(GAMMA_CORRECTION)
    color = vec4(pow(c, vec3(1 / gamma)), 1);
#else
    color = vec4(c, 1);
#endif
}
