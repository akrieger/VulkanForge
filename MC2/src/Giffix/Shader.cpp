#include <cstdlib>

#include "Giffix/Shader.h"

Shader::Shader(Type t, const char* path) : t(t), source(NULL), handle(0), compiled(false) {
  memset(this->path, '\0', sizeof(this->path));
  memcpy(this->path, path, strnlen(path, 63) * sizeof(char));
};

Shader::~Shader() {
  cleanup();
}

void Shader::cleanup() {
  if (source != NULL) {
    free(source);
    source = NULL;
  }
  if (handle) {
    glDeleteShader(handle);
    handle = 0;
  }
}

bool Shader::load() {
  FILE* fptr;
  long length;
 
#pragma warning (push)
#pragma warning (disable: 4996)
  fptr = fopen(path, "rb");
#pragma warning (pop)
  if (fptr == NULL) {
    return false;
  }

  fseek(fptr, 0, SEEK_END);
  length = ftell(fptr);
  source = (char*)malloc(length + 1);
  fseek(fptr, 0, SEEK_SET); 
  fread(source, length, 1, fptr);
  fclose(fptr);
  source[length] = 0;
 
  return true;
}

bool Shader::compile() {
  if (source == NULL) {
    return false;
  }
  
  char* info = NULL;
  GLint is_compiled = 0;
  handle = glCreateShader(t == Type::VERTEX ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
  glShaderSource(handle, 1, (const GLchar**)&source, 0);
  glCompileShader(handle);
  glGetShaderiv(handle, GL_COMPILE_STATUS, &is_compiled);
  if (is_compiled == GL_FALSE) {
    GLsizei length = 0;
		glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &length);
    info = (char*)malloc(length);
    glGetShaderInfoLog(handle, length, &length, info);
    printf("%s\n", info);
    free(info);
    free(source);
    source = NULL;
    return false;
  }
  
  compiled = true;
  return true;
}

bool Shader::isCompiled() const {
  return compiled;
}

GLuint Shader::getHandle() const {
  return handle;
}
