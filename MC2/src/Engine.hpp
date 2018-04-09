#pragma once

#include <GL/glew.h>

#include "Unmigrated/math/vector.hpp"
#include "Renderable.hpp"

class World : public Renderable {
public:
  World(GLuint vertex_vbo, GLuint, int);
  ~World();

  void init();

  void preUpdate(unsigned int ticks); // Perform non-OpenGL updates
  void update(unsigned int ticks);    // Perform OpenGL updates
  void render();                      // Perform OpenGL rendering.

  void reinit(GLuint shader, GLuint vertex_vbo);

private:
  GLuint vao, vbo[3];
  GLuint shader;
  int displacement;
  _462::Vector3* blocks;
  _462::Vector3* colors;
  _462::Vector2* textures;
  GLuint* indices;
};