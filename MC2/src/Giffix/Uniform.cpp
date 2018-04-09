
#include <cassert>

#include "Giffix/ShaderProgram.h"

#include "Giffix/Uniform.hpp"

Uniform::Uniform(UniformType t, handle_t handle, ShaderProgram* shader)
  : t(t), location(handle), shader(shader) {

}
/*
bool Uniform::setValuei(int i) {
  assert(shader->isActive());
  glUniform1i(location, i);
  return true;
}*/
