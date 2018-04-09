#include <cmath>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <stdio.h>

#include "Graphics/cgincludes.h"

#include "Camera/Camera.h"
#include "Camera/FreeCam.h"

#include "Events/PubSub.hpp"

#include "Giffix/Shader.h"
#include "Giffix/ShaderProgram.h"

#include "Graphics/Constants.hpp"
#include "Graphics/GraphicsCore.h"
#include "Graphics/Text.h"

#include "Math/vector.hpp"
#include "Math/matrix.hpp"
#include "Math/quaternion.hpp"

#include "Player/Player.hpp"

#include "Util/generic_util.hpp"

#include "Window/Window.h"

#include "World/World.h"

#include "Visualizer.hpp"

#define VSYNC_ENABLED 1
#define FRAMERATE_LIMITING 1
#define MAX_FPS 60
#define MIN_MS_PER_FRAME (1000.0 / MAX_FPS)

typedef std::mutex mutex_t;
typedef std::condition_variable condvar_t;
typedef std::condition_variable_any condvar_any_t;

static mutex_t run_mutex;

Visualizer::Visualizer() {
  win = NULL;
  cam = new FreeCam();
  running = true;
}

Visualizer::~Visualizer() {
  running = false;
}

bool Visualizer::init() {
  return false;
}

int Visualizer::execute() {
  return 1;
}

void Visualizer::render() {  
  Mat4 mvp = cam->projectionMatrix() * cam->modelViewMatrix();

  sp->setActive();
  GLint loc = sp->getUniform(ShaderConstants.world.uniform_in_MVP);
  GLfloat in_mvp[16];
  mvp.toGLArray(in_mvp);
  glUniformMatrix4fv(loc, 1, GL_FALSE, in_mvp);

  /* Make our background black */
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  for (auto i = renderables.begin(), end = renderables.end(); i != end; i++) {
    (*i)->render();
  }
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glUseProgram(0);
}

void Visualizer::cleanup() {
  glUseProgram(0);

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);

  glDeleteProgram(shaderprogram);

  win->close();
  glfwTerminate();
}

bool Visualizer::isRunning() {
  return running;
}
