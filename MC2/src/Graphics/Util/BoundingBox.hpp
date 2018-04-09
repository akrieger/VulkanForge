#pragma once

#include "Math/math.hpp"

class BoundingBox {
public:
  BoundingBox(real_t lx, real_t ly, real_t lz, real_t ux, real_t uy, real_t uz);
  BoundingBox(real_t coords[6]);
  
  BoundingBox intersect(const BoundingBox& bb);
private:
  real_t lx;
  real_t ly;
  real_t lz;
  real_t ux;
  real_t uy;
  real_t uz;
};