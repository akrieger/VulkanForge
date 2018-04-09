#pragma once

#include <cassert>
#include <cstring>

#include "Math/math.hpp"
#include "Math/vector.hpp"

#include "Util/generic_util.hpp"

/**
 * A 4x4 matrix class. Internal memory layout is column-major, although as it
 * turns out, that doesn't matter as long as you are consistent with which
 * side you operate from. See also
 * http://seanmiddleditch.com/journal/2012/08/matrices-handedness-pre-and-post-multiplication-row-vs-column-major-and-notations/
 *
 * One way to think about it is that the 0th column is the 'x's, the 1st column
 * is the 'y's, etc.
 */
class Mat4 {
public:
  real_t data[4][4];

  Mat4() {
    std::memset(data, 0, sizeof(data));
  }

  Mat4(const Mat4& m) {
    std::memcpy(data, m.data, sizeof(data));
  }

  Mat4(const Mat4&& m) {
    std::memcpy(data, m.data, sizeof(data));
  }

  // Returns the column at index i. No checking.
  inline real_t* operator[](int i) {
    return data[i];
  }

  // Returns the column at i, with checking.
  inline real_t* operator()(int i) {
    assert(0 <= i && i <= 3);
    return data[i];
  }

  // Returns a reference to the element at [i][j]
  inline real_t& operator()(int i, int j) {
    assert(0 <= i && i <= 3 && 0 <= j && j <= 3);
    return data[i][j];
  }

  void transpose() {
    real_t temp[4][4];
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        temp[j][i] = data[i][j];
      }
    }
    memcpy(data, temp, sizeof(data));
  }

  inline const Mat4 operator*(const Mat4& rhs) const {
    Mat4 result;
    memset(result.data, 0, sizeof(result.data));
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        for (int x = 0; x < 4; x++) {
          result.data[i][j] += data[x][j] * rhs.data[i][x];
        }
      }
    }
    return result;
  }

  /**
   * Optimization for transforming a 3D vector by this 4d transformation matrix.
   *
   * (a e i m) * (x) = (ax + ey + iz + m)
   * (b f j n)   (y)   (bx + fy + jz + n)
   * (c g k o)   (z)   (cx + gy + kz + o)
   * (d h l p)   (1)
   */
  inline Vec3 transform(const Vec3& rhs) const {
    return Vec3(
      data[0][0] * rhs.x + data[1][0] * rhs.y + data[2][0] * rhs.z + data[3][0],
      data[0][1] * rhs.x + data[1][1] * rhs.y + data[2][1] * rhs.z + data[3][1],
      data[0][2] * rhs.x + data[1][2] * rhs.y + data[2][2] * rhs.z + data[3][2]
    );
  }
  
  inline Vec3 operator*(const Vec3& rhs) {
    return transform(rhs);
  }

  inline void toGLArray(GLfloat *dest) {
    augment_unchecked((real_t *)data, dest, 16);
  }
};