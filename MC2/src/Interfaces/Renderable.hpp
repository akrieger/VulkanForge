#pragma once

class Renderable {
public:
  virtual void preUpdate(unsigned int ticks) = 0;
  virtual void update(unsigned int ticks) = 0;
  virtual void render() = 0;
private:
};