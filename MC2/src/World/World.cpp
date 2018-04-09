
#include <cstdlib>

#include "World/Chunks/WorldChunk.hpp"
#include "World/World.h"

#include "Util/my_png.h"
#include "Player/Player.hpp"

#include "Giffix/ShaderProgram.h"

#define CHUNK_RADIUS 0

typedef struct vertex {
  unsigned char x;
  unsigned char y;
  unsigned char z;
} vertex;

World* World::singleton;

World::World() : vao(0), shader(0) {
  player = new Player();
  singleton = this;
}

World::~World() {}

void World::init(ShaderProgram* s) {
  shader = s;

  GLuint buffers[3];
  glGenBuffers(3, buffers);

  vertex_buffer = buffers[0];

  // 32x32x32 blocks per chunk
  // 6 faces per block
  // 4 vertices per face
  // each vertex has 3 coordinates (taken care of in sizeof(vertex))
  vertex* vertices = (vertex*)malloc(sizeof(vertex)*32*32*32*6*4);

  unsigned int x,y,z;
  vertex* temp_vertices = vertices;
  vertex raw_vertices[8];
  
  for (y = 0; y < 32; y++) {
    // Some of these will overflow. That's okay. They will not be used.
    // We need to keep consistent numbering.
    raw_vertices[0].y = (unsigned char)y;
    raw_vertices[1].y = (unsigned char)y;
    raw_vertices[2].y = (unsigned char)y;
    raw_vertices[3].y = (unsigned char)y;
    raw_vertices[4].y = (unsigned char)(y + 1);
    raw_vertices[5].y = (unsigned char)(y + 1);
    raw_vertices[6].y = (unsigned char)(y + 1);
    raw_vertices[7].y = (unsigned char)(y + 1);
    for (z = 0; z < 32; z++) {
      raw_vertices[0].z = (unsigned char)z;
      raw_vertices[1].z = (unsigned char)z;
      raw_vertices[2].z = (unsigned char)(z + 1);
      raw_vertices[3].z = (unsigned char)(z + 1);
      raw_vertices[4].z = (unsigned char)z;
      raw_vertices[5].z = (unsigned char)z;
      raw_vertices[6].z = (unsigned char)(z + 1);
      raw_vertices[7].z = (unsigned char)(z + 1);
      for (x = 0; x < 32; x++) {
        raw_vertices[0].x = (unsigned char)x;
        raw_vertices[1].x = (unsigned char)(x + 1);
        raw_vertices[2].x = (unsigned char)x;
        raw_vertices[3].x = (unsigned char)(x + 1);
        raw_vertices[4].x = (unsigned char)x;
        raw_vertices[5].x = (unsigned char)(x + 1);
        raw_vertices[6].x = (unsigned char)x;
        raw_vertices[7].x = (unsigned char)(x + 1);
        
        // Remember: OpenGL is all about CCW winding.
        // Convention: face the face, camera up must point in +y or +z direction.
        // First coordinate is the 'top left' one in this case.
        
        // Bottom face.
        *(temp_vertices++) = raw_vertices[2];
        *(temp_vertices++) = raw_vertices[0];
        *(temp_vertices++) = raw_vertices[1];
        *(temp_vertices++) = raw_vertices[3];
        
        // +z face
        *(temp_vertices++) = raw_vertices[6];
        *(temp_vertices++) = raw_vertices[2];
        *(temp_vertices++) = raw_vertices[3];
        *(temp_vertices++) = raw_vertices[7];
        
        // +x face
        *(temp_vertices++) = raw_vertices[7];
        *(temp_vertices++) = raw_vertices[3];
        *(temp_vertices++) = raw_vertices[1];
        *(temp_vertices++) = raw_vertices[5];
        
        // -z face
        *(temp_vertices++) = raw_vertices[5];
        *(temp_vertices++) = raw_vertices[1];
        *(temp_vertices++) = raw_vertices[0];
        *(temp_vertices++) = raw_vertices[4];
        
        // -x face
        *(temp_vertices++) = raw_vertices[4];
        *(temp_vertices++) = raw_vertices[0];
        *(temp_vertices++) = raw_vertices[2];
        *(temp_vertices++) = raw_vertices[6];
        
        // top face
        *(temp_vertices++) = raw_vertices[7];
        *(temp_vertices++) = raw_vertices[5];
        *(temp_vertices++) = raw_vertices[4];
        *(temp_vertices++) = raw_vertices[6];
      }
    }
  }
  
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*32*32*32*6*4, vertices, GL_STATIC_DRAW);
  free(vertices);
  
  unsigned int width, height;
  int bit_depth;
  glGenTextures(1, &terrain_tex);
  
  unsigned char* tex_bytes = read_png("res/tex/terrain.png", &width, &height, &bit_depth);

  int type = 0;
  switch (bit_depth) {
    case 8: type = GL_UNSIGNED_BYTE; break;
    case 16: type = GL_UNSIGNED_SHORT; break;
    case 32: type = GL_UNSIGNED_INT; break;
  }

  int num_levels = 1;
  while (height > 1) {
    num_levels++;
    height >>= 1;
  }
  height = width;
  
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, terrain_tex);
  glTexStorage2D(GL_TEXTURE_2D, num_levels, GL_RGBA8, width, height);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, type, tex_bytes);
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  GLfloat largest_supported_anisotropy;
  glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest_supported_anisotropy);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, largest_supported_anisotropy);
  
  GLuint tex_loc = shader->getUniform("terrain");
  glUniform1i(tex_loc, 0);
  free(tex_bytes);
  
  chunks.reserve((2 * CHUNK_RADIUS + 1) * (2 * CHUNK_RADIUS + 1));
  
  for (x = -CHUNK_RADIUS; x <= CHUNK_RADIUS; x++) {
    for (z = -CHUNK_RADIUS; z <= CHUNK_RADIUS; z++) {
      chunks.push_back(new WorldChunk(shader, vertex_buffer, x, z));
    }
  }

  for (auto i = chunks.begin(); i != chunks.end(); i++) {
    (*i)->init();
  }
}

void World::preUpdate(unsigned int ticks) {(void)ticks;}

void World::update(unsigned int ticks) {(void)ticks;}

void World::render() {
  shader->setActive();
  for (auto i = chunks.begin(); i != chunks.end(); i++) {
    (*i)->render();
  }
}

Player* World::getPlayer() {
  return World::singleton->player;
}
