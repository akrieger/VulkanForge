#pragma once
 
#include "Graphics/cgincludes.h"
#include <vector>

#include "World/Chunks/WorldChunk.hpp"
#include "World/Direction.hpp"
#include "Interfaces/Renderable.hpp"

class Player;

class World : public Renderable {
 private:
  GLuint vao;
  GLuint vertex_buffer;
  GLuint tex_buffer;
  GLuint terrain_tex;

  ShaderProgram* shader;

  std::vector<WorldChunk*> chunks;

  Player* player;

  static World* singleton;
 public:
  World();
  ~World();

  void preUpdate(unsigned int);
  void update(unsigned int);
  void render();

  void init(ShaderProgram* sp);

  static Player* getPlayer();
};