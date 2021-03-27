#version 450 core

out vec4 FragColor;

in vec3 v_Pos;
in vec2 v_UV0;
in vec3 v_Normal;

uniform sampler2D diffuseTex;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform vec3 lightPos;
uniform vec3 eyePos;
uniform float lightIntensity;
uniform float shininess;

void main()
{
  vec3 color = pow(texture2D(diffuseTex, v_UV0).rgb * kd, vec3(2.2));
  vec3 ambient = ka * color;//环境光

  vec3 lightDir = normalize(lightPos - v_Pos);
  vec3 normal = normalize(v_Normal);
  float lightCoff = lightIntensity / length(lightPos - v_Pos);
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 diffuse =  diff * lightCoff * color;//漫反射

  vec3 viewDir = normalize(eyePos - v_Pos);
  vec3 halfDir = normalize(lightDir + viewDir);
  float spec = pow(max(dot(halfDir, v_Normal), 0), shininess);
  vec3 specular = ks * lightCoff * spec;//高光

  FragColor = vec4(pow(ambient + diffuse + specular, vec3(1.0 / 2.2)), 1.0);
}