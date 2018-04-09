#ifndef _SUBCHUNK_HPP_
#define _SUBCHUNK_HPP_

#include "Graphics/cgincludes.h"

class WorldBlock;
class WorldChunk;

class SubChunk {
public:
  SubChunk(WorldChunk* parent);

  void init(int y, GLuint indices_vbo, GLuint tex_vbo, unsigned int* indices_offset, unsigned int* tex_offset);
  void render();
  void cleanup();

private:
  int index_offset;

  WorldChunk* parent;
  WorldBlock** blocks;
  int y;
  int num_blocks;
};

#endif