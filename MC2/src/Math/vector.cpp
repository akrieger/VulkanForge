#include "Math/quaternion.hpp"

#include "Math/vector.hpp"

const Vec3 Zero = Vec3(0, 0, 0);
const Vec3 X = Vec3(1, 0, 0);
const Vec3 Y = Vec3(0, 1, 0);
const Vec3 Z = Vec3(0, 0, 1);

const Vec3 operator*(real_t t, const Vec3& v) {
  return v * t;
}

void Vec3::operator*=(const Quat& q) {
  *this = (*this) * q;
}

Vec3 Vec3::operator*(const Quat& q) const {
  return q * (*this);
}

