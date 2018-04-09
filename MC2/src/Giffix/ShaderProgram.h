#pragma once

#include <cstdint>
#include <unordered_map>

#include "Giffix/Shader.h"

#include "Graphics/cgincludes.h"

class Uniform;

typedef struct {
  handle_t location;
} Attribute;

class ShaderProgram {
public:
  ShaderProgram();
  ~ShaderProgram();

  bool init();
  bool attach(Shader::Type t, Shader* shader);
  bool link();
  void cleanup();
  
  bool isLinked() const;

  bool setActive();
  bool isActive() const;

  handle_t getHandle() const;

  handle_t getAttribute(const char* name);
  
  handle_t getUniform(const char* name);

private:
  handle_t handle;
  handle_t vertexShader, fragmentShader;

  bool linked;
  bool active;

  uint32_t nextUniformIndex;
  std::unordered_map<uint32_t, Uniform*> uniforms;
  uint32_t nextAttributeIndex;
  std::unordered_map<uint32_t, Attribute> attributes;
};
