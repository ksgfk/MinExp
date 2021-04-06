#version 450 core
layout (location = 0) in vec3 a_Pos;

uniform mat4 lightMVP;

void main() {
  gl_Position = lightMVP * vec4(a_Pos, 1.0f);
}