#ifndef _WORLDCHUNK_HPP_
#define _WORLDCHUNK_HPP_

#include <vector>

#include "Graphics/cgincludes.h"

#include "Interfaces/Renderable.hpp"
#include "Util/perlin.hpp"

class Renderable;
class WorldBlock;
class SubChunk;

class ShaderProgram;
class Uniform;

template <class T>
class Sorter {
public:
  Sorter() {};
  void init(T* data) { this->data = data; };
  bool operator()(int i, int j) {
    return data[i] < data[j];
  };
private:
  T* data;
};

class WorldChunk : public Renderable {
public:
  WorldChunk(ShaderProgram* shader, GLuint vertex_vbo, int x, int z);
  ~WorldChunk();

  void init();

  void preUpdate(unsigned int ticks); // Perform non-OpenGL updates
  void update(unsigned int ticks);    // Perform OpenGL updates
  void render();                      // Perform OpenGL rendering.

  void reinit(GLuint shader, GLuint vertices_lower, GLuint vertices_higher);

private:
  WorldChunk();

  ShaderProgram *shader;
  GLuint vao, vertices, indices, tex;
  GLint locx, locz;
  Uniform* uniform_x;
  Uniform* uniform_z;
  int x, z, num_blocks;

  double distances[32];
  Sorter<double> sorter;
  int rendering_order[32];

  WorldChunk* siblings[4];
  SubChunk** sub_chunks;
//  Chunk* chunks[16];

  //WorldBlock** blocks;

  void addBlock(unsigned short xx, unsigned short yy, unsigned short zz, GLuint indices_vbo, GLuint tex_vbo, unsigned int* indices_offset, unsigned int* tex_offset);
};

#endif