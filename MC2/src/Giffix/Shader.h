#pragma once

#include <cstdint>
#include <vector>

#include "Giffix/Uniform.hpp"

#include "Graphics/cgincludes.h"

class ShaderProgram;

class Shader {
public:
  enum class Type {
    VERTEX,
    FRAGMENT,
  };
  
  
  Shader(Type type, const char *path);
  ~Shader();

  bool load();
  bool compile();
  bool attachTo(ShaderProgram* program);
  void cleanup();

  Type getType() const;
  bool isCompiled() const;

  handle_t getHandle() const;

private:
  Shader(Shader &s);

  char path[64];
  char *source;
  handle_t handle;

  Type t;
  bool compiled;
};
