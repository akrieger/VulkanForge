#version 330

uniform sampler2DRect glyphs;

in vec2 ex_TexCoord;

layout(location = 0) out vec4 out_Color;

void main(void) {
  vec4 sample = texture(glyphs, ex_TexCoord);
  out_Color = vec4(sample.r, 0, 0, sample.r);
}