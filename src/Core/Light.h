#pragma once

#include "MathExt.h"

namespace Mine {

struct PointLight {
  float intensity;
  Vector3 pos;
  Vector3 color;
  constexpr PointLight() : intensity(5),
                           pos(Vector3(-2.9f, 2.9f, 3.2f)),
                           color(Vector3(1, 1, 1)) {}
};

}  // namespace Mine
