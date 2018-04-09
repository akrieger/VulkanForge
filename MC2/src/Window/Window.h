#ifndef _WINDOW_WINDOW_HPP_
#define _WINDOW_WINDOW_HPP_

#include <map>

#include "Graphics/cgincludes.h"

class Window {
public:
  Window();
  Window(int width, int height, const char *title);

  ~Window();

  bool open();
  bool close();

  bool isOpen();

  int getHeight();
  int getWidth();

  bool setHeight(int height);
  bool setWidth(int width);

  void refresh();

  bool shouldClose();

  GLFWwindow* getWindow();

  static Window* handleToWindow(GLFWwindow *gflw_win);
private:
  GLFWwindow* win;
  const char* title;

  int height;
  int width;

  double mouse_x, mouse_y;

  static std::map<GLFWwindow*, Window*> handle_to_window;

  void setupListeners();

#ifdef GLEW_MX
public:
  GLEWContext* getGLEWContext();
private:
  GLEWContext glewContext;
#endif
};


#endif