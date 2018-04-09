#pragma once

#include <unordered_map>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "Events/Types/EventData.h"

#include "Graphics/cgincludes.h"

class Atlas;
class ShaderProgram;
class Window;

class Text {
public:
  static bool init(size_t font_size);
  static void shutdown();

  static void draw(const char* text, size_t n, Window* win, int x, int y);
private:
  static Atlas* atlas;
  
  static ShaderProgram* s;
  static handle_t vao;
  static handle_t glyph_vbo;
  static handle_t glyph_tex;
};
