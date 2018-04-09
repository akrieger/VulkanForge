#include "Text.h"

#include <memory>
#include <unordered_map>

#include "Graphics/cgincludes.h"

#include "Giffix/Shader.h"
#include "Giffix/ShaderProgram.h"

#include "Graphics/Constants.hpp"
#include "Graphics/Text/Atlas.hpp"

#include "Window/Window.h"

ShaderProgram* Text::s = NULL;
handle_t Text::vao = 0;
handle_t Text::glyph_tex = 0;
handle_t Text::glyph_vbo = 0;
Atlas* Text::atlas;

static char printable_letters[] = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*(),./;'[]\\`-=<?>:\"{}|~_+";

template<typename S, typename D>
static void widen(const S* src, D* dst, size_t len) {
  if (!src || !dst || !len) {
    return;
  }
  for (size_t i = 0; i < len; i++) {
    dst[i] = src[i];
  }
}

typedef struct {
  float x, y;
  unsigned int u, v;
} character_vertex;

bool Text::init(size_t font_size) {
  Shader v(Shader::Type::VERTEX, "res/shaders/text.vert");
  v.load();
  v.compile();

  Shader f(Shader::Type::FRAGMENT, "res/shaders/text.frag");
  f.load();
  f.compile();

  s = new ShaderProgram();
  s->attach(Shader::Type::VERTEX, &v);
  s->attach(Shader::Type::FRAGMENT, &f);

  s->init();
  s->link();

  if (!s->isLinked()) {
    delete s;
    s = NULL;
    return false;
  }

  glUseProgram(s->getHandle());

  glGenVertexArrays(1, &vao);
  if (vao == 0) {
    delete s;
    s = NULL;
    return false;
  }
  glBindVertexArray(vao);

  glGenBuffers(1, &glyph_vbo);
  if (glyph_vbo == 0) {
    glBindVertexArray(0);
    glDeleteBuffers(1, &vao);
    delete s;
    s = NULL;
    return false;
  }
   
  glActiveTexture(GL_TEXTURE0 + 3);
  GLint letters_loc = s->getUniform(ShaderConstants.text.attr_glyphs);
  glUniform1i(letters_loc, 3);

  glGenTextures(1, &glyph_tex);
  glBindTexture(GL_TEXTURE_RECTANGLE, glyph_tex);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Clamping to edges is important to prevent artifacts when scaling
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	// Linear filtering usuallv looks best for text
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  std::unique_ptr<FT_ULong[]> unicodes = std::make_unique<FT_ULong[]>(sizeof(FT_ULong) * strlen(printable_letters));
  widen((char*)printable_letters, unicodes.get(), strlen(printable_letters));
  atlas = new Atlas("res/fonts/courier.ttf", (FT_UInt)font_size, std::move(unicodes), strlen(printable_letters), glyph_tex);
  if (!atlas->build()) {
    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_RECTANGLE, 0);
    glDeleteTextures(1, &glyph_tex);
    glBindVertexArray(0);
    glDeleteBuffers(1, &vao);
    delete s;
    s = NULL;
    return false;
  }

  glBindBuffer(GL_ARRAY_BUFFER, glyph_vbo);
  glEnableVertexAttribArray(s->getAttribute(ShaderConstants.text.attr_in_Glyphs));
  glVertexAttribPointer(s->getAttribute(ShaderConstants.text.attr_in_Glyphs), 4, GL_FLOAT, GL_FALSE, 0, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);
  glUseProgram(0);
  
  return true;
}

// Scales a value from [0,dim] to [-1,1]
#define screen_to_unit(v, dim) (float)(((2.0 * (v)) / (dim)) - 1.0)
void Text::draw(const char* text, size_t n, Window* win, int x, int y) {
  glUseProgram(s->getHandle());
  glBindVertexArray(vao); 

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  int width = win->getWidth();
  int height = win->getHeight();

  int count = atlas->glyphCountForString(text, n);
  float* square = (float*)malloc(sizeof(float) * 6 * 4 * count);
  
  // Textures are 0,0 in bottom left, screen is 0,0 in top right.
  // Passed in y coordinate is in screen space, so relative to top of screen.
  // Remap to be relative to bottom of screen.
  int baseline = height - y;
  int origin_x = x, origin_y = baseline;
  float llu, llv, lru, lrv, ulu, ulv, uru, urv;
  float llx, lly, lrx, lry, ulx, uly, urx, ury;
  int char_width;
  int char_height;
  int glyph_num = 0;
  for (size_t i = 0; i < n; i++) {
    if (text[i] == '\n') {
      origin_x = x;
      baseline -= atlas->lineHeight();
      origin_y = baseline;
      continue;
    }

    const CharacterData* cd = atlas->dataForUnicode(text[i]);
    char_width = cd->width;
    char_height = cd->height;

    int mod_x = origin_x + cd->bearing_x;
    // bearing_y for rtl languages is for the top of the letter.
    int mod_y = origin_y + cd->bearing_y;
    llx = screen_to_unit(mod_x, width);
    lly = screen_to_unit(mod_y, height);
    llu = (float)cd->u;
    llv = (float)cd->v;

    lrx = screen_to_unit(mod_x + char_width, width);
    lry = lly;
    lru = (float)(cd->u + char_width);
    lrv = (float)cd->v;

    ulx = llx;
    uly = screen_to_unit(mod_y + char_height, height);
    ulu = (float)cd->u;
    ulv = (float)(cd->v + char_height);

    urx = lrx;
    ury = uly;
    uru = (float)(cd->u + char_width);
    urv = (float)(cd->v + char_height);

    float tris[6][4] = {
      {llx, lly, llu, llv},
      {urx, ury, uru, urv},
      {ulx, uly, ulu, ulv},
      {lrx, lry, lru, lrv},
      {urx, ury, uru, urv},
      {llx, lly, llu, llv}
    };
    memcpy(&square[glyph_num * 6 * 4], tris, sizeof(tris));
    origin_x += cd->advance_x;
    origin_y += cd->advance_y;
    glyph_num++;
  }
  glBindBuffer(GL_ARRAY_BUFFER, glyph_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4 * count, square, GL_DYNAMIC_DRAW);

  glDrawArrays(GL_TRIANGLES, 0, 6 * count);

  //glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  free(square);
  
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  glBindVertexArray(0);
  glUseProgram(0);
}
