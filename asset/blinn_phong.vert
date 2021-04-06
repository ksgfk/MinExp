#version 450 core

#define MAX_LIGHT 5

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec2 a_UV0;
layout (location = 2) in vec3 a_Normal;

uniform mat4 mvp;
uniform mat4 lightMVP[MAX_LIGHT];
uniform int lightCount;

out vec3 v_Pos;
out vec2 v_UV0;
out vec3 v_Normal;
out vec4 v_lightSpacePos[MAX_LIGHT];

void main()
{
  gl_Position = mvp * vec4(a_Pos, 1.0f);
  v_Pos = a_Pos;
  v_UV0 = a_UV0;
  v_Normal = a_Normal;
  for(int i = 0; i < lightCount; i++) {
    v_lightSpacePos[i] = lightMVP[i] * vec4(a_Pos, 1.0f);
  }
}