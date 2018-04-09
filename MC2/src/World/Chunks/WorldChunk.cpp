#include <algorithm>
#include <cstdlib>

#include "Giffix/ShaderProgram.h"
#include "Giffix/Uniform.hpp"

#include "Graphics/Constants.hpp"

#include "Player/Player.hpp"

#include "World/Chunks/SubChunk.hpp"
#include "World/Chunks/WorldChunk.hpp"
#include "World/World.h"
#include "World/WorldBlock.hpp"

#include "Memory/WorldStorage.h"

#define BIAS 0.1;

WorldChunk::WorldChunk(ShaderProgram* s, GLuint v, int x, int z) :
    shader(s), vertices(v), x(x), z(z), num_blocks(0)  {
	
    sorter.init(distances);
    for (int i = 0; i < 32; i++) { rendering_order[i] = i; }
}

WorldChunk::~WorldChunk() {}

void WorldChunk::init() {
  locx = shader->getUniform("chunk_x");
  locz = shader->getUniform("chunk_z");

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vertices);
  glVertexAttribPointer(shader->getAttribute(ShaderConstants.world.attr_in_Position), 3, GL_UNSIGNED_BYTE, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(shader->getAttribute(ShaderConstants.world.attr_in_Position));

  glGenBuffers(1, &indices);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
  // 6 indices per face, b/c 2 triangles with 3 vertices each, 6 faces per block, 16*16*256 blocks per chunk
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6 * 6*32*32*32, NULL, GL_STATIC_DRAW);

  glGenBuffers(1, &tex);
  glBindBuffer(GL_ARRAY_BUFFER, tex);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * 6*32*32*32, NULL, GL_STATIC_DRAW);
  glVertexAttribPointer(shader->getAttribute(ShaderConstants.world.attr_in_Tex), 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(shader->getAttribute(ShaderConstants.world.attr_in_Tex));

  unsigned int indices_offset = 0, tex_offset = 0;                       
  
  sub_chunks = (SubChunk**)malloc(sizeof(SubChunk*) * 1);
  for (int i = 0; i < 1; i++) {
    sub_chunks[i] = new SubChunk(this);
    sub_chunks[i]->init(i, indices, tex, &indices_offset, &tex_offset);
  }
  
  /*
  blocks = (WorldBlock**)malloc(sizeof(WorldChunk*) * 16*16*255);
  for (unsigned char yy = 0; yy < 255; yy++) {
    for (unsigned char zz = 0; zz < 16; zz++) {
      for (unsigned char xx = 0; xx < 16; xx++) {
        blocks[xx + 16 * zz + 256 * yy] = new WorldBlock(this, xx, yy, zz, STONE, 0);
        blocks[xx + 16 * zz + 256 * yy]->addDataToVBOs(indices, tex, &indices_offset, &tex_offset);
      }
    }
  }
  */

  //for (auto i = 0; i < 16 * 16 * 255; i++) {
  //  blocks[i]->addDataToVBOs(indices, tex, &indices_offset, &tex_offset);
  //}
  //num_blocks = indices_offset / 4 / 24;
  
  /*
  double coords[3];
  WorldBlock* temp;
  double noise;
  unsigned int indices_offset = 0, tex_offset = 0;
  for (unsigned char yy = 0; yy < 1; yy++) {
    coords[1] = yy + BIAS;
  for (unsigned char zz = 0; zz < 1; zz++) {
    coords[2] = zz + BIAS;
  for (unsigned char xx = 0; xx < 1; xx++) {
    coords[0] = xx + BIAS;
    noise = p->noise3(coords);
    noise = 1 - noise;
    noise = noise * noise;
    //if (p->noise3(coords) > .2) {
    //  this->addBlock(xx, yy, zz, indices, tex, &indices_offset, &tex_offset);
    //}
  }
  }
  }
  */
  glBindVertexArray(0);
}

void WorldChunk::preUpdate(unsigned int ticks) {(void)ticks;}

void WorldChunk::update(unsigned int ticks) {(void)ticks;}

void WorldChunk::render() {
  glUniform1i(locx, x);
  glUniform1i(locz, z);
  // Order the chunks
  static int counter = 0;
  if (++counter % 1000 == 0) {
    counter = 0;
    Vec3 chunk_pos, temp_vec;
    Vec3 player_pos(World::getPlayer()->getPosition());
    for (int i = 0; i < 1; i++) {
      chunk_pos = Vec3(x * CHUNK_WIDTH + 8, i * 32 + 4, z * 16 * 8);
      temp_vec = player_pos - chunk_pos;
      distances[i] = temp_vec.length2();
    }

    std::sort(&rendering_order[0], &rendering_order[0] + 1, sorter);
  }

  glBindVertexArray(vao);
  for (int i = 0; i < 1; i++) {
    sub_chunks[rendering_order[i]]->render();
  }
  glBindVertexArray(0);
  //glDrawElements(GL_QUADS, num_blocks * 24, GL_UNSIGNED_INT, 0);
}

void WorldChunk::addBlock(unsigned short xx, unsigned short yy, unsigned short zz, GLuint indices_vbo, GLuint tex_vbo, unsigned int* indices_offset, unsigned int* tex_offset) {
  WorldBlock* temp = new WorldBlock(this, xx, yy, zz, STONE, 0);
  temp->addDataToVBOs(indices_vbo, tex_vbo, indices_offset, tex_offset);
  delete temp;
}
