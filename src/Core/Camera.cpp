#include "Camera.h"

#include "Input.h"

using namespace Mine;

void OrbitMotion::UpdateData(int height) {
  auto& input = Input::GetInstance();
  auto&& cPos = input.GetCursePos();
  auto&& curseDelta = Sub(cPos, _cursePos);
  if (input.GetMouse(MouseButton::Left)) {
    _deltaOrbit = Div(curseDelta, (float)height);
  } else {
    _deltaOrbit = Vector2(0, 0);
  }
  if (input.GetMouse(MouseButton::Right)) {
    _deltaPan = Mul(curseDelta, panSpeed);
  } else {
    _deltaPan = Vector2(0, 0);
  }
  _cursePos = cPos;
  _dolly = input.GetScrollDelta();
}

void OrbitMotion::UpdateCamera(Camera& camera) {
  if (!IsEqual(_deltaOrbit.x, 0) || !IsEqual(_deltaOrbit.y, 0) || !IsEqual(_dolly.y, 0)) {
    auto&& fromTarget = Sub(camera.pos, camera.target);
    float radius = Length(fromTarget);
    float theta = std::atan2(fromTarget.x, fromTarget.z);
    float phi = std::acos(fromTarget.y / radius);
    float factor = MINE_PI * 2 * orbitSpeed;
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
  if (!IsEqual(_deltaPan.x, 0) || !IsEqual(_deltaPan.y, 0)) {
    auto&& forward = Sub(camera.target, camera.pos);
    auto&& forwardDir = Normalize(forward);
    auto&& forwardDis = Length(forward);
    auto&& left = Cross(camera.up, forwardDir);
    auto&& up = Cross(forwardDir, left);
    float factor = forwardDis * std::tan(camera.fov / 2.0f) * 2.0f;
    auto&& x = Mul(left, _deltaPan.x * factor);
    auto&& y = Mul(up, _deltaPan.y * factor);
    auto&& delta = Add(x, y);
    camera.target = Add(camera.target, delta);
    camera.pos = Add(camera.pos, delta);
  }
}
