#version 450 core

#define MAX_LIGHT 5

out vec4 FragColor;

in vec3 v_Pos;
in vec2 v_UV0;
in vec3 v_Normal;
in vec4 v_lightSpacePos[MAX_LIGHT];

struct PointLight {
  float intensity;
  vec3 pos;
  vec3 color;
  sampler2D shadowMap;
};

uniform sampler2D diffuseTex;
uniform int hasDiffuseTex;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float shininess;
uniform int lightCount;

uniform vec3 eyePos;
uniform PointLight light[MAX_LIGHT];

#define BIAS 0.001
#define PI 3.141592653589793
#define PI2 6.283185307179586
#define NUM_SAMPLES 96
#define PCF_NUM_SAMPLES NUM_SAMPLES
#define BLOCKER_SEARCH_NUM_SAMPLES NUM_SAMPLES
#define NUM_RINGS 10
#define ZNEAR 0.1
#define LIGHT_SIZE 0.005

vec2 poissonDisk[NUM_SAMPLES];

highp float rand_1to1(highp float x) {
  // -1 -1
  return fract(sin(x)*10000.0);
}

highp float rand_2to1(vec2 uv ) { 
  // 0 - 1
	const highp float a = 12.9898, b = 78.233, c = 43758.5453;
	highp float dt = dot( uv.xy, vec2( a,b ) ), sn = mod( dt, PI );
	return fract(sin(sn) * c);
}

void poissonDiskSamples( const in vec2 randomSeed ) {
  float ANGLE_STEP = PI2 * float( NUM_RINGS ) / float( NUM_SAMPLES );
  float INV_NUM_SAMPLES = 1.0 / float( NUM_SAMPLES );
  float angle = rand_2to1( randomSeed ) * PI2;
  float radius = INV_NUM_SAMPLES;
  float radiusStep = radius;

  for( int i = 0; i < NUM_SAMPLES; i ++ ) {
    poissonDisk[i] = vec2( cos( angle ), sin( angle ) ) * pow( radius, 0.75 );
    radius += radiusStep;
    angle += ANGLE_STEP;
  }
}

vec3 blinnPhong(float intensity, vec3 lightPos, vec3 lightColor, float visibility) {
  vec3 color = pow(texture2D(diffuseTex, v_UV0).rgb, vec3(2.2));
  vec3 ambient = ka * color;//环境光

  vec3 lightDir = normalize(lightPos - v_Pos);
  vec3 normal = normalize(v_Normal);
  float lightCoff = intensity / length(lightPos - v_Pos);
  float diff = max(dot(normal, lightDir), 0);
  vec3 diffuse = kd * diff * lightCoff * color; //漫反射

  vec3 viewDir = normalize(eyePos - v_Pos);
  vec3 halfDir = normalize(lightDir + viewDir);
  float spec = max(pow(dot(halfDir, v_Normal), shininess), 0);
  vec3 specular = ks * lightCoff * spec;//高光

  return pow(ambient + (diffuse + specular) * lightColor * visibility, vec3(1.0 / 2.2));
}

float shadowMap(sampler2D map, vec3 shadowCoord) {
  float depth = texture2D(map, shadowCoord.xy).r;
  return depth + BIAS < shadowCoord.z ? 0 : 1;
}

float pcf(sampler2D map, vec3 shadowCoord, float filterSize) {
  float sum = 0.0;
  for(int i = 0; i < PCF_NUM_SAMPLES; i++) {
    vec2 pos = poissonDisk[i] * filterSize;
    float depth = texture2D(map, pos + shadowCoord.xy).r;
    if(depth + BIAS > shadowCoord.z) {
      sum += 1.0;
    }
  }
  float result = sum / float(PCF_NUM_SAMPLES);
  return result;
}

vec2 findBlocker(sampler2D map, vec2 uv, float zReceiver) {
  float search = LIGHT_SIZE * (zReceiver - ZNEAR) / zReceiver;
  float allDepth = 0.0;
  float blockNum = 0.0;
  for(int i = 0; i < BLOCKER_SEARCH_NUM_SAMPLES; i++) {
    vec2 pos = uv + poissonDisk[i] * search;
    float depth = texture2D(map, pos).r;
    if (depth < zReceiver) {
      allDepth += depth;
      blockNum += 1.0;
    }
  }
  float avgDep = allDepth / blockNum;
	return vec2(avgDep, blockNum);
}

float pcss(sampler2D map, vec3 shadowCoord) {
  vec2 blocker = findBlocker(map, shadowCoord.xy, shadowCoord.z);
  if(blocker.y < 1.0) {
    return 1.0;
  }
  float proportion = (shadowCoord.z - blocker.x) / blocker.x;
  float penumbra = proportion * (LIGHT_SIZE * ZNEAR) / shadowCoord.z;
  float result = pcf(map, shadowCoord, penumbra);
  return result;
}

void main()
{
  vec3 result;
  for(int i = 0; i < lightCount; i++) {
    vec4 lightSpacePos = v_lightSpacePos[i];
    vec3 homoCrop = lightSpacePos.xyz / lightSpacePos.w;
    vec3 depthSpace = homoCrop * 0.5 + 0.5;
    poissonDiskSamples(depthSpace.xy);
    // float visibable = shadowMap(light[i].shadowMap, depthSpace);
    // float visibable = pcf(light[i].shadowMap, depthSpace, 0.001);
    float visibable = pcss(light[i].shadowMap, depthSpace);
    result += blinnPhong(light[i].intensity, light[i].pos, light[i].color, visibable);
  }
  FragColor = vec4(result, 1);
}