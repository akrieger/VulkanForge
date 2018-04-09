#pragma once

#include <math.h>

#include "Math/math.hpp"
#include "Math/matrix.hpp"

class Vec3;

class Quat {
public:
  real_t w,x,y,z;

  Quat() : w(1), x(0), y(0), z(0) {}
  Quat(real_t w, real_t x, real_t y, real_t z) : w(w), x(x), y(y), z(z) {}
  Quat(real_t theta, const Vec3& axis);
  
  
  inline void mul(real_t m) {
    operator*=(Quat(m, 0, 0, 0));
  }

  /*
   *(a,v)*(b,w) = (ab - v.w, a*w + b*v + v x w)
   */
  inline void operator*=(const Quat& q) {
    real_t ww, xx, yy, zz;
    ww = (w * q.w) - (x * q.x - y * q.y - z * q.z);
    xx = (w * q.x) + (x * q.w) + (y * q.z - z * q.y);
    yy = (w * q.y) + (y * q.w) - (x * q.z + z * q.x);
    zz = (w * q.z) + (z * q.w) + (x * q.y - y * q.x);
    w = ww;
    x = xx;
    y = yy;
    z = zz;
  }

  inline Quat operator*(const Quat& q) const {
    real_t ww, xx, yy, zz;
    ww = (w * q.w) - (x * q.x - y * q.y - z * q.z);
    xx = (w * q.x) + (x * q.w) + (y * q.z - z * q.y);
    yy = (w * q.y) + (y * q.w) - (x * q.z + z * q.x);
    zz = (w * q.z) + (z * q.w) + (x * q.y - y * q.x);
    return Quat(
      ww,
      xx,
      yy,
      zz
    );
  }

  inline real_t norm() const {
    return w * w + x * x + y * y + z * z;
  }

  inline void normalize() {
    real_t len = norm();
    w /= len;
    x /= len;
    y /= len;
    z /= len;
  }

  /**
   * From http://gns.wikia.com/wiki/Quaternions and other places too.
   */
  inline Vec3 operator*(const Vec3& v) const {
    // qv = 2 * q x v
    Vec3 q(x,y,z);
    Vec3 qv = q.cross(v) * 2;
    // qqv = q x qv
    Vec3 qqv = q.cross(qv);
  
    // return v + w * qv + qqv
    return v + (qv * w) + qqv;
  }

  inline void toMatrix(Mat4& out) const;
};