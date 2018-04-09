#pragma once

#undef GLEW_MX
#ifdef GLEW_MX

extern "C" {
  GLEWContext* glewGetContext();
}

#endif

#ifdef _WIN32
#define GLEW_STATIC
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>

typedef GLuint handle_t;