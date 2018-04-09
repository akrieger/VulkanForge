#pragma once

#include "Math/vector.hpp"

class Player {
public:
  Player();
  Vec3 getPosition();
  void setPosition(const Vec3&);
private:
  Vec3 position;
};