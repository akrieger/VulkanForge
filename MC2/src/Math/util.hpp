#pragma once

#include "Math/math.hpp"

class Vec3;
class Mat4;

void glhTranslate(Mat4& matrix, const Vec3& v);

void lookAt(const Vec3& eye, const Vec3& target, const Vec3& up, Mat4& out);

void glhFrustum(real_t left, real_t right, real_t bottom, real_t top,
                real_t znear, real_t zfar, Mat4& matrix);

void frustrum(real_t fovyInDegrees, real_t aspectRatio,
              real_t znear, real_t zfar, Mat4& matrix);