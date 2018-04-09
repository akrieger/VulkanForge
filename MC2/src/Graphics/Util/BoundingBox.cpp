#include <string.h>

#include "BoundingBox.hpp"

BoundingBox::BoundingBox(real_t lx, real_t ly, real_t lz, real_t ux, real_t uy, real_t uz) : lx(lx), ly(ly), lz(lz), ux(ux), uy(uy), uz(uz) { }

BoundingBox::BoundingBox(real_t coords[6]) {
  lx = coords[0];
  ly = coords[1];
  lz = coords[2];
  ux = coords[3];
  uy = coords[4];
  uz = coords[6];
}