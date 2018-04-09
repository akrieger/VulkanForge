#pragma once

struct ShaderConstants_t {
  struct {
    const char* uniform_in_MVP;
    const char* uniform_in_chunk_x;
    const char* uniform_in_chunk_y;
    const char* uniform_in_chunk_z;
    const char* attr_in_Position;
    const char* attr_in_Tex;
  } world;
  struct {
    const char* attr_in_Glyphs;
    const char* attr_glyphs;
  } text;
};

extern const struct ShaderConstants_t ShaderConstants;