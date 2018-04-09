#include <cstdlib>

#include "Graphics/cgincludes.h"

#include "Memory/WorldStorage.h"
#include "World/WorldBlock.hpp"

#include "World/Chunks/SubChunk.hpp"

SubChunk::SubChunk(WorldChunk* parent) : y(-1), num_blocks(0), parent(parent) { }

void SubChunk::init(int new_y, GLuint indices_vbo, GLuint tex_vbo, unsigned int* indices_offset, unsigned int* tex_offset) {
  y = new_y;
  index_offset = *indices_offset;
  blocks = (WorldBlock**)malloc(sizeof(WorldBlock*) * 32 * 32 * 32);

  block_index_t x, yy, z;
  for (block_index_t i = 0; i < 32*32*32; i++) {
    x = i % 32;
    yy = i / (32 * 32);
    z = (i / 32) % 32;
    blocks[i] = new WorldBlock(parent, x, yy, z, STONE, 0);
  }
  
  for (block_index_t i = 0; i < 32*32*32; i++) {
    blocks[i]->addDataToVBOs(indices_vbo, tex_vbo, indices_offset, tex_offset);
  }
  
  num_blocks = (*indices_offset - index_offset) / 4 / 36;
}

void SubChunk::render() {
  glDrawElements(GL_TRIANGLES, num_blocks * 6 * 2 * 3, GL_UNSIGNED_INT, reinterpret_cast<const void*>(index_offset));
  //glDrawRangeElements(GL_QUADS, y * 16 * 255, ((y + 1) * 16 * 255) - 1, num_blocks * 6 * 4, GL_UNSIGNED_INT, (const GLvoid*)index_offset);
}
