#pragma once

#include "MathExt.h"

namespace Mine {

struct Camera {
  Vector3 pos;
  Vector3 target;
  Vector3 up;
  float aspect;
  float zNear;
  float zFar;
  float fov;
  constexpr Camera() : pos(Vector3(3, 3, -3)),
                       target(Vector3(0, 0, 0)),
                       up(Vector3(0, 1, 0)),
                       aspect(16.0f / 9.0f),
                       zFar(50.0f),
                       zNear(0.1f),
                       fov(ToRadians(60.0f)) {}

  constexpr Matrix4x4 View() const { return LookAtRH(pos, target, up); }
  inline Matrix4x4 Projection() const {
    return PerspectiveRH(fov, aspect, zNear, zFar);
    // return Mine::OrthoRH(-10, 10, -10, 10, 0.1f, 100);
  }
};

/*
 * https://github.com/mrdoob/three.js/blob/master/examples/js/controls/OrbitControls.js
 */
class OrbitMotion {
 private:
  Vector2 _cursePos;
  Vector2 _deltaOrbit;
  Vector2 _deltaPan;
  Vector2 _dolly;

 public:
  float panSpeed = 0.0008f;
  float orbitSpeed = 0.5f;

  void UpdateData(int height);
  void UpdateCamera(Camera& camera);
};

}  // namespace Mine
