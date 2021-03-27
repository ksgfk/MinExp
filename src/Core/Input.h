#pragma once

#include <array>
#include <utility>

#include "MathExt.h"

namespace Mine {

enum class KeyCode {
  A,
  B,
  C,
  D,
  E,
  F,
  G,
  H,
  I,
  J,
  K,
  L,
  M,
  N,
  O,
  P,
  Q,
  R,
  S,
  T,
  U,
  V,
  W,
  X,
  Y,
  Z,
  SIZE
};

enum class MouseButton {
  Left,
  Right,
  Middle,
  SIZE
};

using KeyStateArray = std::array<bool, (int)KeyCode::SIZE>;
using ButtonStateArray = std::array<bool, (int)MouseButton::SIZE>;

class Input {
 private:
  Input();

  KeyStateArray _keyNowState;
  KeyStateArray _keyDownState;
  KeyStateArray _keyUpState;
  ButtonStateArray _msNowState;
  ButtonStateArray _msDownState;
  ButtonStateArray _msUpState;
  Vector2 _mousePos;
  Vector2 _scrollNowDelta;
  Vector2 _scrollDelta;

 public:
  Input(const Input&) = delete;
  Input(Input&&) = delete;
  Input& operator=(const Input&) = delete;
  Input& operator=(Input&&) = delete;

  static Input& GetInstance() {
    static Input input;
    return input;
  }

  void UpdateState();
  bool GetKey(KeyCode keyCode) const;
  bool GetKeyDown(KeyCode keyCode) const;
  bool GetKeyUp(KeyCode keyCode) const;
  bool GetMouse(MouseButton msBtn) const;
  bool GetMouseDown(MouseButton msBtn) const;
  bool GetMouseUp(MouseButton msBtn) const;
  Vector2 GetCursePos() const;
  Vector2 GetScrollDelta() const;
};

}  // namespace Mine
