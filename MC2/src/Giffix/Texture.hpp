#pragma once

#include "Graphics/cgincludes.h"

typedef enum {
  SQUARE = GL_TEXTURE_2D,
  RECTANGLE = GL_TEXTURE_RECTANGLE,
  CUBE = GL_TEXTURE_CUBE_MAP,
} TextureType;

class Texture {
public:
  Texture(TextureType t);
  ~Texture();

  bool initialize();

  bool isValid() const;
  handle_t getHandle() const;

private:
  TextureType type;
  handle_t handle;
};