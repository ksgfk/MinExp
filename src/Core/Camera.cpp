#include "Camera.h"

#include "Input.h"

using namespace Mine;

void OrbitMotion::UpdateData(int height) {
  auto& input = Input::GetInstance();
  auto&& cPos = input.GetCursePos();
  if (input.GetMouse(MouseButton::Left)) {
    _deltaOrbit = Div(Sub(cPos, _pos), (float)height);
  }
  if (input.GetMouse(MouseButton::Right)) {
    _deltaLook = Div(Sub(cPos, _pos), (float)height);
  } else {
    _deltaLook = Vector2(0, 0);
  }
  _pos = cPos;
  _dolly = input.GetScrollDelta();
}

void OrbitMotion::UpdateCamera(Camera& camera) {
  if (!IsEqual(_deltaOrbit.x, 0) || !IsEqual(_deltaOrbit.y, 0) || !IsEqual(_dolly.y, 0)) {
    auto&& fromTarget = Sub(camera.pos, camera.target);
    float radius = Length(fromTarget);
    float theta = std::atan2(fromTarget.x, fromTarget.z);
    float phi = std::acos(fromTarget.y / radius);
    float factor = MINE_PI * 2;
    radius *= std::pow(0.95f, _dolly.y);
    auto orbit = _deltaOrbit;
    auto t = theta - orbit.x * factor;
    auto p = phi - orbit.y * factor;
    p = Clamp(p, MINE_EPSILON, MINE_PI - MINE_EPSILON);
    Vector3 offset;
    offset.x = radius * (float)sin(p) * (float)sin(t);
    offset.y = radius * (float)cos(p);
    offset.z = radius * (float)sin(p) * (float)cos(t);
    camera.pos = Add(camera.target, offset);
  }

  if (!IsEqual(_deltaLook.x, 0) || !IsEqual(_deltaLook.y, 0)) {
    auto&& fromCamera = Sub(camera.target, camera.pos);
    float radius = Length(fromCamera);
    float theta = std::atan2(fromCamera.x, fromCamera.z);
    float phi = std::acos(fromCamera.y / radius);
    float factor = MINE_PI * 2;
    auto look = _deltaLook;
    auto t = theta + look.x * factor;
    auto p = phi - look.y * factor;
    p = Clamp(p, MINE_EPSILON, MINE_PI - MINE_EPSILON);
    Vector3 offset;
    offset.x = radius * (float)sin(p) * (float)sin(t);
    offset.y = radius * (float)cos(p);
    offset.z = radius * (float)sin(p) * (float)cos(t);
    camera.target = Add(camera.pos, offset);
  }
}
