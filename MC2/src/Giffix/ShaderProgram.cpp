#include <cstdlib>
#include <vector>
  
#include "Giffix/Uniform.hpp"

#include "Giffix/ShaderProgram.h"

ShaderProgram::ShaderProgram() : handle(0), vertexShader(0), fragmentShader(0),
  linked(false), active(false), nextUniformIndex(0), nextAttributeIndex(0) {}

ShaderProgram::~ShaderProgram() {
  uniforms.clear();
  attributes.clear();
  if (handle) {
    glDeleteProgram(handle);
    
    glDetachShader(handle, vertexShader);
    glDetachShader(handle, fragmentShader);
    handle = 0;
  }
}

void ShaderProgram::cleanup() {
}

bool ShaderProgram::init() {
  handle = glCreateProgram();
  return handle != 0;
}

bool ShaderProgram::attach(Shader::Type t, Shader* shader) {
  if (shader == NULL) {
    return false;
  }
  if (!shader->isCompiled()) {
    return false;
  }

  if (t == Shader::Type::VERTEX) {
    vertexShader = shader->getHandle();
    return (vertexShader != 0);
  } else if (t == Shader::Type::FRAGMENT) {
    fragmentShader = shader->getHandle();
    return (vertexShader != 0);
  }

  return false;
}

bool ShaderProgram::link() {
  if (vertexShader == 0 || fragmentShader == 0) {
    return false;
  }
  if (handle == 0) {
    return false;
  }

  glAttachShader(handle, vertexShader);
  glAttachShader(handle, fragmentShader);
  glLinkProgram(handle);
   
  GLint is_linked = 0;
  glGetProgramiv(handle, GL_LINK_STATUS, (int*)&is_linked);
  if (is_linked == GL_FALSE) {
    char* info = NULL;
    GLsizei length = 0;

    glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length);
    info = (char*)malloc(length);

    glGetProgramInfoLog(handle, length, &length, info);
    printf("%s\n", info);

    free(info);
    return false;
  }

  linked = true;
  return true;
}

bool ShaderProgram::isLinked() const {
  return linked;
}

bool ShaderProgram::setActive() {
  if (handle == 0) {
    return false;
  }

  glUseProgram(handle);
  active = true;
  return true;
}

bool ShaderProgram::isActive() const {
  return active;
}

handle_t ShaderProgram::getHandle() const {
  return handle;
}
  
handle_t ShaderProgram::getUniform(const char* name) {
  return glGetUniformLocation(handle, name);
}

handle_t ShaderProgram::getAttribute(const char* name) {
  return glGetAttribLocation(handle, name);
}
