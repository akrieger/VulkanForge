#pragma once

#include "Graphics/cgincludes.h"

#define explode2(v) v.x,v.y
#define explode3(v) v.x,v.y,v.z

#define explode2p(v, p) v.x p, v.y p
#define explode3p(v, p) v.x p, v.y p, v.z p

void sleep_for(int millis);

// Converts an array of one type into an array of another type.
// Assumes that S can be constructed from D.
// Suppresses conversion warnings.
template<typename S, typename D>
void augment_unchecked(const S* src, D* dst, size_t len) {
  if (!src || !dst || !len) {
    return;
  }
#pragma warning (push)
#pragma warning (disable: 4244)
  for (size_t i = 0; i < len; i++) {
    dst[i] = (D)src[i];
  }
#pragma warning (pop)
}

template<typename S, typename D>
void augment(const S* src, D* dst, size_t len) {
  if (!src || !dst || !len) {
    return;
  }
  for (size_t i = 0; i < len; i++) {
    dst[i] = src[i];
  }
}