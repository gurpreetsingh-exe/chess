#version 460 core

out vec2 uv;

void main(void) {
  float x = float((gl_VertexID & 1) << 2);
  float y = float((gl_VertexID & 2) << 1);
  uv = .5f * vec2(x, y);
  gl_Position = vec4(x - 1.f, y - 1.f, 0.f, 1.f);
}
