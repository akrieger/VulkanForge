#include "Window/Window.h"

#include "Graphics/cgincludes.h"

#include "Events/Types/Events.h"
#include "Events/Types/EventData.h"
#include "Events/PubSub.hpp"

#include "Graphics/GraphicsCore.h"

static void mouse_move_listener(GLFWwindow *win, double x_new, double y_new);
static void mouse_click_listener(GLFWwindow *win, int button, int action, int mods);
static void keyboard_listener(GLFWwindow *win, int key, int scancode, int action, int mods);
static void framebuffer_size_listener(GLFWwindow *win, int width, int height);

std::map<GLFWwindow*, Window*> Window::handle_to_window;

Window::Window() :
    width(0), height(0), mouse_x(-1), mouse_y(-1), title(""), win(NULL) {
      
}

Window::Window(int width, int height, const char* title) :
    width(width), height(height), mouse_x(-1), mouse_y(-1), title(title), win(NULL) {

}

Window::~Window() {
  close();
}

bool Window::open() {
  win = glfwCreateWindow(width, height, title, NULL, NULL);
  if (win) {
    GraphicsCore::registerOpenWindow(this);
    setupListeners();
    handle_to_window[win] = this;
    return true;
  }
  return false;
}

bool Window::close() {
  if (win) {
    glfwDestroyWindow(win);
    GraphicsCore::registerClosedWindow(this);
    handle_to_window.erase(handle_to_window.find(win));
  }
  return true;
}

bool Window::isOpen() {
  return getWindow() != NULL;
}

void Window::setupListeners() {
  if (win) {
    glfwSetFramebufferSizeCallback(win, &framebuffer_size_listener);
    glfwSetMouseButtonCallback(win, &mouse_click_listener);
    glfwSetCursorPosCallback(win, &mouse_move_listener);
    glfwSetKeyCallback(win, &keyboard_listener);
  }
}

int Window::getWidth() {
  glfwGetWindowSize(win, &width, &height);
  return width;
}

int Window::getHeight() {
  glfwGetWindowSize(win, &width, &height);
  return height;
}

bool Window::setWidth(int w) {
  if (win) {
    return false;
  }
  width = w;
  return true;
}

bool Window::setHeight(int h) {
  if (win) {
    return false;
  }
  height = h;
  return true;
}

void Window::refresh() {
  glfwSwapBuffers(win);
}

bool Window::shouldClose() {
  return glfwWindowShouldClose(win) != 0;
}

GLFWwindow* Window::getWindow() {
  return win;
}

Window* Window::handleToWindow(GLFWwindow *win) {
  auto res = handle_to_window.find(win);
  if (res != handle_to_window.end()) {
    return res->second;
  }
  return NULL;
}

static void framebuffer_size_listener(GLFWwindow *win, int width, int height) {
  GLFWwindow* current_window = glfwGetCurrentContext();
  glfwMakeContextCurrent(win);
  glViewport(0, 0, width, height);
  glfwMakeContextCurrent(current_window);
}

static void mouse_click_listener(GLFWwindow *win, int button, int action, int mods) {
  EventData d;
  MouseClickData& mcd = d.MouseClickData;
  switch (action) {
    case GLFW_PRESS:
      mcd.action = KeyDown;
      break;
    case GLFW_RELEASE:
      mcd.action = KeyUp;
      break;
  }
  mcd.button = (MouseButton)button;
  mcd.modifiers = mods;
  PubSub::get().pub(MouseClick, &d, Window::handleToWindow(win));
}

static void mouse_move_listener(GLFWwindow *win, double x_new, double y_new) {
  static double x_old = -1, y_old = -1;

  if (x_old == -1 || y_old == -1) {
    x_old = x_new;
    y_old = y_new;
    return;
  }

  EVENT_STRUCT(MouseMoveData, d, mmd);

  mmd.dx = x_new - x_old;
  mmd.dy = y_new - y_old;
  
  mmd.x = x_old = x_new;
  mmd.y = y_old = y_new;

  PubSub::get().pub(MouseMove, &d, Window::handleToWindow(win));
}

static void keyboard_listener(GLFWwindow *win, int key, int scancode, int action, int mods) {
  EVENT_STRUCT(KeyPressData, d, kpd);

  switch (action) {
    case GLFW_PRESS:
      kpd.action = KeyDown;
      break;
    case GLFW_REPEAT:
      kpd.action = KeyRepeat;
      break;
    case GLFW_RELEASE:
      kpd.action = KeyUp;
      break;
  }
  kpd.key = (KeyboardKey)key;
  kpd.scancode = scancode;
  kpd.modifiers = mods;
  PubSub::get().pub(KeyPress, &d, Window::handleToWindow(win));
}

#ifdef GLEW_MX
extern "C" {
  GLEWContext *glewGetContext() {
    return GraphicsCore::getCurrentWindow()->getGLEWContext();
  }
}
#endif
