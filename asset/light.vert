#version 450 core
layout (location = 0) in vec3 a_Pos;

uniform mat4 mvp;

void main() {
  gl_Position = mvp * vec4(a_Pos, 1.0f);
}