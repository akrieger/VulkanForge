//
//  main.cpp
//  MC2
//
//  Created by Andrew Krieger on 11/12/16.
//  Copyright © 2016 akrieger.org. All rights reserved.
//

#include <stdlib.h>

#include "Application/Application.h"

#include "Graphics/GraphicsCore.h"

namespace {
  void die(int code, const char *desc) {
    printf("%d: %s\n", code, desc);
    glfwTerminate();
    exit(code);
  }

  bool bootstrap() {
    if (!GraphicsCore::initWindowing()) {
      return false;
    }
    
    glfwSetErrorCallback(&die);
    
    glfwWindowHint(GLFW_RESIZABLE, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.2
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    
    return true;
  }
}

int main(int, char **) {
  Application app;
  
  if (!bootstrap()) {
    return -1;
  }
  
  return app.run();
}
