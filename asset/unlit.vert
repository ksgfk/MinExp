#version 450 core
layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec2 a_UV0;
layout (location = 2) in vec3 a_Normal;

uniform mat4 mvp;

out vec3 v_Pos;
out vec2 v_UV0;
out vec3 v_Normal;

void main()
{
  gl_Position = mvp * vec4(a_Pos, 1.0f);
  v_Pos = a_Pos;
  v_UV0 = a_UV0;
  v_Normal = a_Normal;
}