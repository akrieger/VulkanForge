#include <cmath>
#include <unordered_map>
#include <set>
#include <tuple>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "Graphics/Text/Atlas.hpp"

static FT_Library library;
static bool initialized = false;

bool initFreeType() {
  if (!initialized) {
    int error = FT_Init_FreeType(&library);
    if (!error) {
      initialized = true;
    }
  }
  return initialized;
}

void cleanupFreeType() {
  if (initialized) {
    FT_Done_FreeType(library);
    initialized = false;
  }
}

// A FreeRect represents the area a glyph can be placed, automatically
// including a 1px border between any other glyphs.
typedef struct FreeRect {
  int llx, lly, urx, ury;

  FreeRect(int _llx, int _lly, int _urx, int _ury) : llx(_llx), lly(_lly), urx(_urx), ury(_ury) {}
  FreeRect(const CharacterData& cd) {
    llx = cd.u - 1;
    lly = cd.v - 1;
    urx = cd.u + cd.width + 1;
    ury = cd.v + cd.height + 1;
  }
  // Sort operator for a multiset, optimizes the maxrects packing algorithm.
  bool operator<(const struct FreeRect& rhs) const {
    int delta = (urx - llx) - (rhs.urx - rhs.llx);
    return delta != 0 ? delta < 0 : ((ury - lly) - (rhs.ury - rhs.lly)) < 0;
  };

  bool intersects(const struct FreeRect& rhs) const {
    bool vertical = false, horizontal = false;
    // top slice
    vertical = ury > rhs.lly && lly < rhs.ury;
    horizontal = urx > rhs.llx && llx < rhs.urx;

    return vertical && horizontal;
  }

  bool containsStrictly(const struct FreeRect& rhs) const {
    int ldx = llx - rhs.llx;
    int ldy = lly - rhs.lly;
    int udx = urx - rhs.urx;
    int udy = ury - rhs.ury;
    
    return
      (ldx || ldy || udx || udy) && // Can't be equal.
      ldx <= 0 &&
      ldy <= 0 &&
      udx >= 0 &&
      udy >= 0;
  }

  bool containsOrEquals(const struct FreeRect& rhs) const {
    return
      llx <= rhs.llx &&
      lly <= rhs.lly &&
      urx >= rhs.urx &&
      ury >= rhs.ury;
  }

} FreeRect;

static inline int top(const FreeRect& fr) {
  return fr.ury;
}

static inline int bottom(const FreeRect& fr) {
  return fr.lly;
}

static inline int left(const FreeRect& fr) {
  return fr.llx;
}

static inline int right(const FreeRect& fr) {
  return fr.urx;
}

/**
 * Iterates over all the FreeRects in fl, intersects them with fr, and adds the all results to fl_next.
 */
static void intersectAll(const FreeRect& fr, std::multiset<FreeRect>* fl, std::multiset<FreeRect>* fl_next) {  
  for (auto i = fl->begin(), end = fl->end(); i != end; i++) {
    bool top_slice = false, bottom_slice = false, right_slice = false, left_slice = false;
    bool vertical_overlap = false, horizontal_overlap = false;
    const FreeRect& fr_old = *i;
    bool sliced = false;

    vertical_overlap = top(fr) > bottom(fr_old) && bottom(fr) < top(fr_old);
    horizontal_overlap = left(fr) < right(fr_old) && right(fr) > left(fr_old); 
    // top slice
    if (top(fr) < top(fr_old) && top(fr) > bottom(fr_old)) {
      top_slice = true;
    }

    // bottom slice
    if (bottom(fr) > bottom(fr_old) && bottom(fr) < top(fr_old)) {
      bottom_slice = true;
    }

    // left slice
    if (left(fr) > left(fr_old) && left(fr) < right(fr_old)) {
      left_slice = true;
    }

    // right slice
    if (right(fr) < right(fr_old) && right(fr) > left(fr_old)) {
      right_slice = true;
    }

    if (horizontal_overlap) {
      // Can only have horizontal slices if the left or right intrude.
      if (top_slice) {
        fl_next->emplace(left(fr_old), top(fr), right(fr_old), top(fr_old));
        sliced = true;
      }
      if (bottom_slice) {
        fl_next->emplace(left(fr_old), bottom(fr_old), right(fr_old), bottom(fr));
        sliced = true;
      }
    }

    if (vertical_overlap) {
      // Can only have vertical slices if the top or bottom intrude.
      if (left_slice) {
        fl_next->emplace(left(fr_old), bottom(fr_old), left(fr), top(fr_old));
        sliced = true;
      }
      if (right_slice) {
        fl_next->emplace(right(fr), bottom(fr_old), right(fr_old), top(fr_old));
        sliced = true;
      }
    }

    if (!sliced) {
      fl_next->insert(fr_old);
    }
  }
}

static void deduplicate(std::multiset<FreeRect>* fl, std::multiset<FreeRect>* fl_next) {
  for (auto i = fl->begin(), end = fl->end(); i != end; i++) {
    const FreeRect& tester = *i;
    bool keep = true;

    // Go through 'keep' list first. It's short and likely has large items in it. We use
    // a stronger comparison here so we avoid inserting identical FreeRects.
    for (auto ii = fl_next->begin(), eend = fl_next->end(); keep && ii != eend; ii++) {
      keep = !ii->containsOrEquals(tester);
    }

    // Go through the rest next. We use a weaker comparison here because if we get to
    // this point, we know that an identical FreeRect has not already been kept.
    for (auto ii = fl->begin(), eend = fl->end(); keep && ii != eend; ii++) {
      if (i == ii) {
        continue;
      }
      keep = !ii->containsStrictly(tester);
    }
    if (keep) {
      fl_next->insert(tester);
    }
  }
}

Atlas::Atlas(const char* font, FT_UInt font_size, std::unique_ptr<FT_ULong[]>&& unicodes, size_t count, handle_t tex) :
font(font), font_size(font_size), unicodes(std::move(unicodes)), count(count), built(false), tex(tex) {}

Atlas::~Atlas() { }

bool Atlas::build() {
  size_t width = 0, height = 0;
  
  if (tex == 0) {
    return false;
  }
  initFreeType();

  int error = FT_New_Face(library, font, 0, &face);
  if (error == FT_Err_Unknown_File_Format) {
    return false;
  } else if (error) {
    return false;
  }

  error = FT_Set_Pixel_Sizes(face, 0, font_size);
  if (error) {
    return false;
  }

  kerning = (FT_HAS_KERNING(face) != 0);
  line_height = face->size->metrics.height;
  
  loadGlyphStatsFromFont();

  layoutGlyphs(&width, &height);

  glBindTexture(GL_TEXTURE_RECTANGLE, tex);
  glTexImage2D(
    GL_TEXTURE_RECTANGLE, 0,
    GL_RED, (GLsizei)width, (GLsizei)height, 0,
    GL_RED, GL_UNSIGNED_BYTE, NULL
  );
  
  blitToTexture();

  FT_Done_Face(face);
  cleanupFreeType();
  return true;
}

bool Atlas::loadGlyphStatsFromFont() {
  int error = 0;
  size_t num_distinct_glyphs = 0;
  FT_ULong unicode;
  FT_UInt glyph_index;
  FT_Glyph glyph;
  FT_BitmapGlyph bm_glyph;

  for (size_t i = 0; i < count; i++) {
    unicode = unicodes[i];
    glyph_index = FT_Get_Char_Index(face, unicode);
    if (letters.find(glyph_index) != letters.end()) {
      continue;
    }

    num_distinct_glyphs++;
    
    error = FT_Load_Char(face, unicode, FT_LOAD_DEFAULT);
    if (error) {
      continue;
    }
  
    error = FT_Get_Glyph(face->glyph, &glyph);
    if (error) {
      FT_Done_Glyph(glyph);
      continue;
    }

    error = FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, true);
    if (error) {
      FT_Done_Glyph(glyph);
      continue;
    }
    
    bm_glyph = (FT_BitmapGlyph)glyph;
    CharacterData& c = letters[glyph_index];
    unicodeToGlyphIndex[unicode] = glyph_index;

    c.unicode = unicode;
    c.glyph_index = glyph_index;

    c.width = bm_glyph->bitmap.width;
    c.height = bm_glyph->bitmap.rows;

    c.advance_x = (bm_glyph->root.advance.x >> 16);
    c.advance_y = (bm_glyph->root.advance.y >> 16);

    c.bearing_x = bm_glyph->left;
    c.bearing_y = bm_glyph->top - c.height;

    c.u = -1;
    c.v = -1;

    FT_Done_Glyph(glyph);
  }

  return true;
}

bool Atlas::layoutGlyphs(size_t* width, size_t* height) {
  // Using MaxRects-BSSF-BBF-DESCSS, from http://clb.demon.fi/files/RectangleBinPack.pdf
  std::multiset<FreeRect> fl1, fl2;
  auto freelist = &fl1;
  auto next_freelist = &fl2;

  bool retryLarger;
  size_t currentSize = 64;
  do {
    freelist->emplace(0, 0, currentSize, currentSize);
    retryLarger = false;
    for (auto i = unicodeToGlyphIndex.begin(), end = unicodeToGlyphIndex.end(); i != end; i++) {
      CharacterData& c = letters[i->second];
      auto f = freelist->end();
      auto eend = f;

      for (auto ii = freelist->begin(); ii != eend; ii++) {
        if (ii->urx - ii->llx >= c.width + 2 && ii->ury - ii->lly >= c.height + 2) {
          f = ii;
          break;
        }
      }

      if (f == eend) {
        retryLarger = true;
        currentSize += 64;
        freelist->clear();
        next_freelist->clear();
        break;
      }

      const FreeRect& fr = *f;

      c.u = fr.llx + 1;
      c.v = fr.lly + 1;

      // Create disjoint freerects from emplaced rect.
      intersectAll(FreeRect(fr.llx, fr.lly, fr.llx + c.width + 2, fr.lly + c.height + 2), freelist, next_freelist);
      freelist->clear();
      deduplicate(next_freelist, freelist);
      next_freelist->clear();
    }
  } while (retryLarger && currentSize > 0);

  // Verify.
  for (auto i = unicodeToGlyphIndex.begin(), end = unicodeToGlyphIndex.end(); i != end; i++) {
    auto ii = i;
    ii++;
    for (; ii != end; ii++) {
      if (FreeRect(letters[i->second]).intersects(FreeRect(letters[ii->second]))) {
        printf("Intersecting letters!\n");
      }
    }
  }

  *height = currentSize;
  *width = currentSize;

  return false;
}

bool Atlas::blitToTexture() {
  FT_ULong unicode = 0;
  FT_UInt glyph_index = 0;
  int error = 0;
  
  CharacterData c;  
  FT_Glyph glyph;
  FT_BitmapGlyph bm_glyph;

  for (size_t i = 0; i < count; i++) {
    unicode = unicodes[i];
    if (unicodeToGlyphIndex.find(unicode) == unicodeToGlyphIndex.end()) {
      continue;
    }

    glyph_index = unicodeToGlyphIndex[unicode];
    
    error = FT_Load_Char(face, unicode, FT_LOAD_DEFAULT);
    if (error) {
      continue;
    }
  
    error = FT_Get_Glyph(face->glyph, &glyph);
    if (error) {
      FT_Done_Glyph(glyph);
      continue;
    }

    error = FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, true);
    if (error) {
      // Not sure how this can happen.
      FT_Done_Glyph(glyph);
      continue;
    }
    bm_glyph = (FT_BitmapGlyph)glyph;
    c = letters[glyph_index]; 

    // We have to do this row by row because we want the bottom of the glyph
    // to be oriented towards the v = 0 edge of the texture.
    for (int height = 0; height < c.height; height++) {
      glTexSubImage2D(
        GL_TEXTURE_RECTANGLE, 0,
        c.u, c.v + height,
        c.width, 1,
        GL_RED, GL_UNSIGNED_BYTE,
        &bm_glyph->bitmap.buffer[c.width * (c.height - height - 1)]
      );
    }
    
    FT_Done_Glyph(glyph);
  }

  return true;
}

int Atlas::glyphCountForString(const char* text, size_t n) const {
  int count = (int)n;
  for (size_t i = 0; i < n; i++) {
    switch(text[i]) {
      case '\n':
        count--;
        break;
      default:
        break;
    }
  }
  return count;
}

FT_ULong Atlas::lineHeight() const {
  return line_height / 64;
}

const CharacterData* Atlas::dataForUnicode(FT_ULong unicode) const {
  auto i = unicodeToGlyphIndex.find(unicode);
  if (i == unicodeToGlyphIndex.end()) {
    return NULL;
  }
  FT_UInt ii = i->second;
  return &letters.at(ii);
}
