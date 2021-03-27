#pragma once

#include "MathExt.h"

namespace Mine {

constexpr const char* LIGHT_KA = "ka";
constexpr const char* LIGHT_KD = "kd";
constexpr const char* LIGHT_KS = "ks";
constexpr const char* LIGHT_POS = "lightPos";
constexpr const char* LIGHT_INTENSITY = "lightIntensity";
constexpr const char* LIGHT_EYEPOS = "eyePos";
constexpr const char* LIGHT_SHININESS = "shininess";

struct PointLight {
  float intensity;
  Vector3 pos;
  Vector3 ka;
  Vector3 kd;
  Vector3 ks;
  float shininess;
  constexpr PointLight() : intensity(5),
                           pos(Vector3(-2.9f, 2.9f, 3.2f)),
                           ka(Vector3(0.05f, 0.05f, 0.05f)),
                           kd(Vector3(1, 1, 1)),
                           ks(Vector3(1, 1, 1)),
                           shininess(64) {}
};

}  // namespace Mine
