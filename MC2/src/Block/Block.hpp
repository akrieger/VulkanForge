#pragma once

#ifndef NDEBUG
#define NEDBUG_ONLY(x) 
#else
#define NDEBUG_ONLY
#endif

typedef struct {
  int type:3;
  int extra:4;
  int solid:1;
} BlockData;