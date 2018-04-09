#pragma once

#include <stdio.h>

#include <vector>

#include "Graphics/cgincludes.h"

class Window;

class GraphicsCore {
public:
  static bool initWindowing();
  static bool initGlew();
  static bool initText();
  static bool shutdown();

  static int getGLMajorVersion() {
    return glMajorVersion;
  }

  static int getGLMinorVersion() {
    return glMinorVersion;
  }

  static int getMaxAALevel() {
    return maxAALevel;
  }

  static bool isVSyncEnabled() {
    return vsyncEnabled;
  }

  static void registerOpenWindow(Window *win);
  static void registerClosedWindow(Window *win);

private:
  GraphicsCore();
  ~GraphicsCore();

  static int glMajorVersion;
  static int glMinorVersion;

  static int maxAALevel;

  static bool vsyncEnabled;

  static std::vector<Window*> windows;

  static void errorFunc(int code, const char* desc) {
    printf("%d: %s\n", code, desc);
  }
};
