#include "Memory/WorldStorage.h"

#include "World/WorldBlock.hpp"

WorldBlock::WorldBlock(WorldChunk* p, unsigned short x, unsigned short y, unsigned short z, BlockID id, unsigned char meta)
  : parent(p), x(x), y(y), z(z), id(id), meta(meta) { }

void WorldBlock::addDataToVBOs(GLuint indices, GLuint tex, unsigned int* indices_offset, unsigned int* tex_offset) const {
  // Assume no culling for now.

  float tex_coords[8] = {0.0,1, 0.0,0, 1,0, 1,1};
  for (int i = 0; i < 6; i++) {
    glBufferSubData(GL_ARRAY_BUFFER, *tex_offset, sizeof(float) * 8, tex_coords);
    *tex_offset += 8 * sizeof(float);
  }

  // Gotta short-circuit this.
  if (y > 254) {
    return;
  }

  if (!(x == 0 || x == 31 || z == 0 || z == 31 || y == 0 || y == 31)) {
    return;
  }

  int base_index = 0;
  base_index += x;
  base_index += 32 * 32 * y;
  base_index += 32 * z;
  base_index *= 24; // each block has 6 faces, with 4 vertices per face
                    // That's assuming it's a 'regular' block though B|
                    // Friggin Notch.

  unsigned int block_indices[36];

  int offset = 0, index_offset = 0;
  for (int i = 0; i < 6; i++) {
    block_indices[offset+0] = base_index + index_offset + 3;
    block_indices[offset+1] = base_index + index_offset + 0;
    block_indices[offset+2] = base_index + index_offset + 2;
    block_indices[offset+3] = base_index + index_offset + 2;
    block_indices[offset+4] = base_index + index_offset + 0;
    block_indices[offset+5] = base_index + index_offset + 1;
    offset += 6;
    index_offset += 4;
  }

  //float tex_coords[8] = {1/16.0,1.0,1/16.0,15/16.0,2/16.0,15/16.0,2/16.0,1.0};

  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, *indices_offset, sizeof(unsigned int) * 36, block_indices); 
  *indices_offset += 36 * sizeof(unsigned int);
}