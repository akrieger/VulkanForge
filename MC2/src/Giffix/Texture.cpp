
#include "Giffix/Texture.hpp"

Texture::Texture(TextureType t) : type(t), handle(0) {}

Texture::~Texture() {
  if (handle) {
    glDeleteTextures(1, &handle);
    handle = 0;
  }
}

bool Texture::initialize() {
  return false;
}

bool Texture::isValid() const {
  return handle != 0;
}

handle_t Texture::getHandle() const {
  return handle;
}