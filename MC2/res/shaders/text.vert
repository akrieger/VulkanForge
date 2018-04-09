#version 150

in vec4 in_Glyphs;

out vec2 ex_TexCoord;

void main(void) {
  gl_Position = vec4(in_Glyphs.xy, 0, 1);
  ex_TexCoord = in_Glyphs.zw;
}