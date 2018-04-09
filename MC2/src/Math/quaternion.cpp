#include "Math/math.hpp"
#include "Math/vector.hpp"

#include "Math/quaternion.hpp"

Quat::Quat(real_t radians, const Vec3& axis) {
  w = cos(radians / 2.0);
  Vec3 norm(axis);
  norm.normalize();
  real_t sine = sin(radians / 2.0);
  x = norm.x * sine;
  y = norm.y * sine;
  z = norm.z * sine;
}

/**
   * Adapted from code in http://www.cs.ucr.edu/~vbz/resources/quatut.pdf
   */
inline void Quat::toMatrix(Mat4& out) const {
  real_t len = norm();
  if (len <= 0.0) {
    memset(out.data, 0, sizeof(out.data));
    out[3][3] = 1.0;
    return;
  }
  real_t s = 2.0 / len;
  real_t xs = x*s,  ys = y*s,  zs = z*s;
  real_t wx = w*xs, wy = w*ys, wz = w*zs;
  real_t xx = x*xs, xy = x*ys, xz = x*zs;
  real_t yy = y*ys, yz = y*zs, zz = z*zs;
    
  out[0][0] = 1.0 - (yy + zz); out[1][0] = xy - wz;         out[2][0] = xz + wy;         out[3][0] = 0.0; 
  out[0][1] = xy + wz;         out[1][1] = 1.0 - (xx + zz); out[2][1] = yz - wx;         out[3][1] = 0.0;
  out[0][2] = xz - wy;         out[1][2] = yz + wx;         out[2][2] = 1.0 - (xx + yy); out[3][2] = 0.0;
  out[0][3] = 0.0;             out[1][3] = 0.0;             out[2][3] = 0.0;             out[3][3] = 1.0;
}