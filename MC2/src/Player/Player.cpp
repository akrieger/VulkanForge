#include "Math/vector.hpp"

#include "Player/Player.hpp"

Player::Player() : position(0,0,0) { }

Vec3 Player::getPosition() {
  return position;
}

void Player::setPosition(const Vec3& new_pos) {
  position = new_pos;
}