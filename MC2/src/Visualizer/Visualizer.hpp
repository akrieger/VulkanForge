#pragma once

#include <vector>

#include "Graphics/cgincludes.h"

#include "World/World.h"

class Window;
class Camera;

class Visualizer {
 private:
  bool running;
 
  Camera *cam;
  Window *win;

  //GLuint vao, vbo[3];
 
	/* These are handles used to reference the shaders */
  //GLuint vertexshader, fragmentshader;
 
	/* This is a handle to the shader program */
  GLuint shaderprogram;

  ShaderProgram* sp;

  GLint displacementLocation;

  World* world;

  std::vector<Renderable*> renderables;

 public:
  Visualizer();
  ~Visualizer();

  bool init();

  int execute();

  void loop();
  void render();
  void cleanup();

  /* Accessors */

  bool isRunning();
};
