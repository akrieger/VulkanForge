#ifndef _MATH_VECTOR_HPP_
#define _MATH_VECTOR_HPP_

#include <math.h>

#include "Math/math.hpp"

class Quat;

class Vec3 {
public:
  real_t x, y, z;

  static const Vec3 X, Y, Z;

  Vec3() : x(0), y(0), z(0) {}
  Vec3(real_t x, real_t y, real_t z) : x(x), y(y), z(z) {}
  Vec3(const Vec3& v) : x(v.x), y(v.y), z(v.z) {}

  inline void operator+=(real_t t) {
    x += t;
    y += t;
    z += t;
  }

  inline Vec3 operator+(real_t t) const {
    return Vec3(
      x + t,
      y + t,
      z + t
    );
  }

  inline void operator+=(const Vec3& v) {
    x += v.x;
    y += v.y;
    z += v.z;
  }

  inline Vec3 operator+(const Vec3& v) const {
    return Vec3(
      x + v.x,
      y + v.y,
      z + v.z
    );
  }

  inline void operator-=(real_t t) {
    x -= t;
    y -= t;
    z -= t;
  }

  inline Vec3 operator-(real_t t) const {
    return Vec3(
      x - t,
      y - t,
      z - t
    );
  }

  inline void operator-=(const Vec3& v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
  }

  inline Vec3 operator-(const Vec3& v) const {
    return Vec3(
      x - v.x,
      y - v.y,
      z - v.z
    );
  }

  inline void operator*=(real_t t) {
    x *= t;
    y *= t;
    z *= t;
  }

  inline Vec3 operator*(real_t t) const {
    return Vec3(
      x * t,
      y * t,
      z * t
    );
  }

  inline void operator*=(const Vec3& v) {
    x *= v.x;
    y *= v.y;
    z *= v.z;
  }

  inline Vec3 operator*(const Vec3& v) const {
    return Vec3(
      x * v.x,
      y * v.y,
      z * v.z
    );
  }

  void operator*=(const Quat& q);
  Vec3 operator*(const Quat& q) const;

  inline void operator/=(real_t t) {
    x /= t;
    y /= t;
    z /= t;
  }

  inline Vec3 operator/(real_t t) const {
    return Vec3(
      x / t,
      y / t,
      z / t
    );
  }

  inline real_t length2() const {
    return x * x + y * y + z * z;
  }

  inline real_t length() const {
    return sqrt(length2());
  }

  inline void normalize() {
    operator/=(length());
  }
  
  inline real_t& operator[](int i) {
    return (&x)[i];
  }

  inline real_t operator[](int i) const {
    return (&x)[i];
  }


  inline real_t dot(const Vec3& v) const {
    return x * v.x + y * v.y + z * v.z;
  }

  inline Vec3 cross(const Vec3& v) const {
    return Vec3(
      y * v.z - z * v.y,
      z * v.x - x * v.z, // Don't forget the negation.
      x * v.y - y * v.x
    );
  }

  // In radians.
  inline real_t cos_between(const Vec3& v) {
    return dot(v) / (length() * v.length());
  }
};

const Vec3 operator*(real_t t, const Vec3& v);

#endif