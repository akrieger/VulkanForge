#pragma once

#include <functional>
#include <vector>

#include "Graphics/UI/UI.hpp"

class UI;

typedef int dimension_t;

class QuadTree {
public:
  QuadTree(dimension_t maxX, dimension_t maxY);
  
  QuadTree *newQuadTree(dimension_t maxX, dimension_t maxY);
  virtual ~QuadTree() = 0;
  
  virtual void add(const UI *ui) = 0;
  virtual void remove(const UI *ui) = 0;
  virtual std::vector<const UI *> elementsAtPoint(int x, int y) const = 0;
};
