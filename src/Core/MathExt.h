#pragma once

#include <cmath>

namespace Mine {

constexpr float MINE_PI = 3.1415927f;
constexpr float MINE_EPSILON = 1e-5f;

constexpr float ToRadians(float degrees) { return ((MINE_PI / 180.0f) * (degrees)); }

constexpr float ToDegrees(float radians) { return ((180.0f / MINE_PI) * (radians)); }

constexpr float Clamp(float f, float min, float max) { return f < min ? min : (f > max ? max : f); }

inline bool IsEqual(float a, float b) { return std::abs(a - b) < MINE_EPSILON; }

struct Vector2 {
  float x, y;
  constexpr Vector2() : Vector2(0, 0) {}
  constexpr Vector2(float x, float y) : x(x), y(y) {}
};

struct Vector3 {
  float x, y, z;
  constexpr Vector3() : Vector3(0, 0, 0) {}
  constexpr Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
};

struct Vector4 {
  float x, y, z, w;
  constexpr Vector4() : Vector4(0, 0, 0, 0) {}
  constexpr Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
};

constexpr Vector2 Add(const Vector2& a, const Vector2& b) { return Vector2(a.x + b.x, a.y + b.y); }
constexpr Vector2 Sub(const Vector2& a, const Vector2& b) { return Vector2(a.x - b.x, a.y - b.y); }
constexpr Vector2 Mul(const Vector2& v, float factor) { return Vector2(v.x * factor, v.y * factor); }
constexpr Vector2 Div(const Vector2& v, float divisor) { return Mul(v, 1.0f / divisor); }
constexpr float Dot(const Vector2& a, const Vector2& b) { return a.x * b.x + a.y * b.y; }
inline float Length(const Vector2& v) { return std::sqrt(Dot(v, v)); }
constexpr Vector2 Normalize(const Vector2& v) { return Div(v, Length(v)); }

constexpr Vector3 Add(const Vector3& a, const Vector3& b) { return Vector3(a.x + b.x, a.y + b.y, a.z + b.z); }
constexpr Vector3 Sub(const Vector3& a, const Vector3& b) { return Vector3(a.x - b.x, a.y - b.y, a.z - b.z); }
constexpr Vector3 Mul(const Vector3& v, float factor) { return Vector3(v.x * factor, v.y * factor, v.z * factor); }
constexpr Vector3 Div(const Vector3& v, float divisor) { return Mul(v, 1.0f / divisor); }
constexpr float Dot(const Vector3& a, const Vector3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
constexpr Vector3 Cross(const Vector3& a, const Vector3& b) {
  float x = a.y * b.z - a.z * b.y;
  float y = a.z * b.x - a.x * b.z;
  float z = a.x * b.y - a.y * b.x;
  return Vector3(x, y, z);
}
inline float Length(const Vector3& v) { return std::sqrt(Dot(v, v)); }
constexpr Vector3 Normalize(const Vector3& v) { return Div(v, Length(v)); }

struct Matrix3x3 {
  float m11;
  float m21;
  float m31;
  float m12;
  float m22;
  float m32;
  float m13;
  float m23;
  float m33;
  constexpr Matrix3x3() : Matrix3x3(0, 0, 0, 0, 0, 0, 0, 0, 0) {}
  constexpr Matrix3x3(float m11, float m12, float m13,
                      float m21, float m22, float m23,
                      float m31, float m32, float m33)
      : m11(m11), m12(m12), m13(m13),
        m21(m21), m22(m22), m23(m23),
        m31(m31), m32(m32), m33(m33) {}

  constexpr static Matrix3x3 Identity() {
    return Matrix3x3(1, 0, 0,
                     0, 1, 0,
                     0, 0, 1);
  }
};

//at beginning.i know matrix of OpenGL is column major.and i want to use constexpr.
//so i write all member variables one by one.
//until i know the function glUniformMatrix4fv can set matrix transpose...
// :(
//and i dont know how to use C++ template
//so.let it go...
struct Matrix4x4 {
  float m11;
  float m21;
  float m31;
  float m41;
  float m12;
  float m22;
  float m32;
  float m42;
  float m13;
  float m23;
  float m33;
  float m43;
  float m14;
  float m24;
  float m34;
  float m44;
  constexpr Matrix4x4() : Matrix4x4(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0) {}
  constexpr Matrix4x4(float m11, float m12, float m13, float m14,
                      float m21, float m22, float m23, float m24,
                      float m31, float m32, float m33, float m34,
                      float m41, float m42, float m43, float m44)
      : m11(m11), m12(m12), m13(m13), m14(m14),
        m21(m21), m22(m22), m23(m23), m24(m24),
        m31(m31), m32(m32), m33(m33), m34(m34),
        m41(m41), m42(m42), m43(m43), m44(m44) {}
  constexpr static Matrix4x4 Identity() {
    return Matrix4x4(1, 0, 0, 0,
                     0, 1, 0, 0,
                     0, 0, 1, 0,
                     0, 0, 0, 1);
  }
};

constexpr Matrix4x4 Mul(const Matrix4x4& a, const Matrix4x4& b) {
  return Matrix4x4((a.m11 * b.m11 + a.m12 * b.m21 + a.m13 * b.m31 + a.m14 * b.m41),
                   (a.m11 * b.m12 + a.m12 * b.m22 + a.m13 * b.m32 + a.m14 * b.m42),
                   (a.m11 * b.m13 + a.m12 * b.m23 + a.m13 * b.m33 + a.m14 * b.m43),
                   (a.m11 * b.m14 + a.m12 * b.m24 + a.m13 * b.m34 + a.m14 * b.m44),
                   (a.m21 * b.m11 + a.m22 * b.m21 + a.m23 * b.m31 + a.m24 * b.m41),
                   (a.m21 * b.m12 + a.m22 * b.m22 + a.m23 * b.m32 + a.m24 * b.m42),
                   (a.m21 * b.m13 + a.m22 * b.m23 + a.m23 * b.m33 + a.m24 * b.m43),
                   (a.m21 * b.m14 + a.m22 * b.m24 + a.m23 * b.m34 + a.m24 * b.m44),
                   (a.m31 * b.m11 + a.m32 * b.m21 + a.m33 * b.m31 + a.m34 * b.m41),
                   (a.m31 * b.m12 + a.m32 * b.m22 + a.m33 * b.m32 + a.m34 * b.m42),
                   (a.m31 * b.m13 + a.m32 * b.m23 + a.m33 * b.m33 + a.m34 * b.m43),
                   (a.m31 * b.m14 + a.m32 * b.m24 + a.m33 * b.m34 + a.m34 * b.m44),
                   (a.m41 * b.m11 + a.m42 * b.m21 + a.m43 * b.m31 + a.m44 * b.m41),
                   (a.m41 * b.m12 + a.m42 * b.m22 + a.m43 * b.m32 + a.m44 * b.m42),
                   (a.m41 * b.m13 + a.m42 * b.m23 + a.m43 * b.m33 + a.m44 * b.m43),
                   (a.m41 * b.m14 + a.m42 * b.m24 + a.m43 * b.m34 + a.m44 * b.m44));
}

constexpr Matrix4x4 LookAtRH(const Vector3& eyePos, const Vector3& target, const Vector3& up) {
  auto&& f = Normalize(Sub(target, eyePos));
  auto&& r = Normalize(Cross(f, up));
  auto&& u = Cross(r, f);
  return Matrix4x4(r.x, r.y, r.z, -Dot(r, eyePos),
                   u.x, u.y, u.z, -Dot(u, eyePos),
                   -f.x, -f.y, -f.z, Dot(f, eyePos),
                   0, 0, 0, 1);
}

inline Matrix4x4 PerspectiveRH(float fov, float aspectRatio, float zNear, float zFar) {
  float n = zNear;
  float f = zFar;
  float t = n * std::tan(fov / 2.0f);
  float b = -t;
  float r = aspectRatio * t;
  float l = -r;
  Matrix4x4 trans(2.0f / (r - l), 0, 0, 0,
                  0, 2.0f / (t - b), 0, 0,
                  0, 0, 2.0f / (f - n), 0,
                  0, 0, 0, 1);
  Matrix4x4 scale(1, 0, 0, -(r + l) / 2.0f,
                  0, 1, 0, -(t + b) / 2.0f,
                  0, 0, 1, -(f + n) / 2.0f,
                  0, 0, 0, 1);
  Matrix4x4 persp(n, 0, 0, 0,
                  0, n, 0, 0,
                  0, 0, -(n + f), -(n * f),
                  0, 0, -1, 0);
  return Mul(Mul(trans, scale), persp);
}

}  // namespace Mine
