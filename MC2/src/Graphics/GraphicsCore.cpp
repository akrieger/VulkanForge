#include "Graphics/cgincludes.h"

#include "Graphics/Text.h"

#include "Window/Window.h"

#include "Graphics/GraphicsCore.h"

std::vector<Window*> GraphicsCore::windows;

bool GraphicsCore::initWindowing() {
  return glfwInit() == GL_TRUE;
}

bool GraphicsCore::initGlew() {  
  glewExperimental = true;
  auto ret = glewInit();
  while (glGetError()); // Piss off glew.
  return ret == GLEW_OK;
}

bool GraphicsCore::initText() {
  return Text::init(60);
}

bool GraphicsCore::shutdown() {
  glfwTerminate();
  return true;
}

void GraphicsCore::registerOpenWindow(Window *win) {
  static bool __HACK_glew_initialized = false;
  windows.push_back(win);
  if (!__HACK_glew_initialized) {
    glfwMakeContextCurrent(win->getWindow());
    initGlew();
    __HACK_glew_initialized = true;
  }
}

void GraphicsCore::registerClosedWindow(Window *win) {
  for (auto i = windows.begin(), end = windows.end(); i != end; i++) {
    if ((*i) == win) {
      windows.erase(i);
      return;
    }
  }
}
