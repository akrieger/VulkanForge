#pragma once

#include <memory>
#include <unordered_map>
#include <set>

#include "Graphics/cgincludes.h"

#include <ft2build.h>
#include FT_FREETYPE_H

typedef struct {
  float x, y;
  float u, v;
} TextVertex;

typedef struct {
  TextVertex ll1, ur1, ul;
  TextVertex lr, ur2, ll2;
} GlyphData;

typedef struct {
  FT_ULong unicode;
  int glyph_index;
  
  // u and v coordinates of the bottom-left corner of the glyph.
  // Assume glyph extends from (u,v) to (u+width, v+height)
  int u, v;

  int texture_offset; // deprecated 

  // These are the dimensions of the glyph in the texture. In a 1:1 mapping,
  // these are also the rendered dimensions in pixels on screen.
  int width;
  int height;

  // How many glyph-pixels to move the cursor after printing this character.
  FT_Pos advance_x;
  FT_Pos advance_y; // 0 for rtl languages, probably.

  // Offset from pen location to right side of glyph.
  int bearing_x;
  // Offset from pen location to bottom side of glyph. Deviation from FreeType.
  int bearing_y;
} CharacterData;

class Atlas {
public:
  Atlas(const char* font, FT_UInt font_size, std::unique_ptr<FT_ULong[]>&& unicodes, size_t count, handle_t tex);
  ~Atlas();

  /**
   * Builds a texture atlas for the given list of unicode characters.
   * It is the callers responsibility to setup all GL state necessary
   * before calling this function (glActiveTexture, glBindTexture).
   */
  bool build();

  FT_ULong lineHeight() const;
  const CharacterData* dataForUnicode(FT_ULong unicode) const;

  int pixelWidthForString(const char* text, size_t n) const;
  int glyphCountForString(const char* text, size_t n) const;

  GLuint getTextureHandle() const;
private:
  const char* font;
  FT_UInt font_size;
  std::unique_ptr<FT_ULong[]> unicodes;
  size_t count;

  FT_Face face;
  GLuint tex;
  bool kerning;

  bool built;
  
  FT_ULong line_height;
  std::unordered_map<FT_ULong, FT_UInt> unicodeToGlyphIndex;
  std::unordered_map<FT_UInt, CharacterData> letters;

  bool loadGlyphStatsFromFont();
  bool layoutGlyphs(size_t* width, size_t* height);
  bool blitToTexture();
};