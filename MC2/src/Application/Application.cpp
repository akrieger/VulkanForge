//
//  Application.cpp
//  MC2
//
//  Created by Andrew Krieger on 11/12/16.
//  Copyright © 2016 akrieger.org. All rights reserved.
//

#include <stdlib.h>

#include "Application.h"

#include "Camera/Camera.h"
#include "Camera/FreeCam.h"

#include "Giffix/Shader.h"
#include "Giffix/ShaderProgram.h"

#include "Graphics/GraphicsCore.h"
#include "Graphics/Constants.hpp"
#include "Graphics/Text.h"

#include "Interfaces/Renderable.hpp"

#include "Math/matrix.hpp"

#include "Window/Window.h"

#include "World/World.h"

namespace {  
  constexpr bool VSYNC_ENABLED = true;
}

int Application::run() {
  if (init()) {
    int ret = execute();
    cleanup();
    return ret;
  }
  return 1;
}

bool Application::init() { 
  mainWindow_ = std::make_shared<Window>(768, 768, "Application");
  mainWindow_->open();
  if (!mainWindow_->isOpen()) {
    return false;
  }
  
  glfwMakeContextCurrent(mainWindow_->getWindow());
  glfwSwapInterval(VSYNC_ENABLED);
  
  mainCamera_ = std::make_shared<FreeCam>();
  mainCamera_->attachWindow(mainWindow_.get());
  
  GraphicsCore::initGlew();
  GraphicsCore::initText();
  
  const char* vertexpath = "res/shaders/world.vert";
  const char* fragmentpath = "res/shaders/world.frag";
  
  Shader v(Shader::Type::VERTEX, vertexpath);
  v.load();
  v.compile();
  
  Shader f(Shader::Type::FRAGMENT, fragmentpath);
  f.load();
  f.compile();
  
  shaderProgram_ = std::make_shared<ShaderProgram>();
  shaderProgram_->attach(Shader::Type::VERTEX, &v);
  shaderProgram_->attach(Shader::Type::FRAGMENT, &f);
  
  shaderProgram_->init();
  shaderProgram_->link();
  if (!shaderProgram_->isLinked()) {
    return false;
  }
  
  glUseProgram(shaderProgram_->getHandle());
  
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  
  world_ = std::make_shared<World>();
  
  world_->init(shaderProgram_.get());
  
  renderables_.push_back(world_);
  
  glUseProgram(0);
  
  return true;
}

int Application::execute() {
  running_ = init();
 	if (!running_) {
    return 1;
  }
  
  double start(0), ticks(0), end(0), sample_start(0), sample_end(0);
  int counter = 0;
  double running_fps = 0.0;
  char fps[6];
  int printed_chars;
  sample_start = glfwGetTime();
  while (running_) {
    glfwPollEvents();
    if (mainWindow_->shouldClose()) {
      running_ = false;
      return 0;
    }
    
    if (start != 0.0) {
      EVENT_STRUCT(TimerData, d, td);
      ticks = glfwGetTime() - start;
      td.dt = ticks;
      PubSub::get().pub(Timer, &d);
    }
    start = glfwGetTime();
    render();
    printed_chars = snprintf(fps, 6, "%f", running_fps);
    fps[5] = '\0';
    if (printed_chars > 0) {
      Text::draw(fps, std::min(printed_chars, 5), mainWindow_.get(), 0, 60);
    }
    
    mainWindow_->refresh();
    
    end = glfwGetTime();
    ticks = end - start;
    if (++counter % 100 == 0) {
      sample_end = glfwGetTime();
      running_fps = (100.0 / (sample_end - sample_start));
      counter = 0;
      sample_start = sample_end;
    }
#if FRAMERATE_LIMITING
    if (ticks < MIN_MS_PER_FRAME) {
      sleep_for((int)(MIN_MS_PER_FRAME - (ticks * 1000) - 1));
      ticks = MIN_MS_PER_FRAME;
    } else {
      //sleep_for(1);
    }
#endif
  }
  
  cleanup();
  
  return 0;
}

void Application::render() {
  Mat4 mvp = mainCamera_->projectionMatrix() * mainCamera_->modelViewMatrix();
  
  shaderProgram_->setActive();
  GLint loc = shaderProgram_->getUniform(ShaderConstants.world.uniform_in_MVP);
  GLfloat in_mvp[16];
  mvp.toGLArray(in_mvp);
  glUniformMatrix4fv(loc, 1, GL_FALSE, in_mvp);
  
  /* Make our background black */
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  for (auto& i : renderables_) {
    i->render();
  }
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glUseProgram(0);
}

void Application::cleanup() {
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  
  shaderProgram_.reset();
  
  mainWindow_->close();
  glfwTerminate();
}
