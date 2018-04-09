#version 330

uniform sampler2D terrain;

in vec3 ex_Color;
in vec2 ex_TexCoord;

layout(location = 0) out vec4 out_Color;

void main(void) {
  vec4 color = texture(terrain, ex_TexCoord.st);
  color.a = 1.0;
  //if (color.a < 0.9) { discard; }
  out_Color = color;// + vec4(ex_Color, 0.0);
  //gl_FragColor = vec4(ex_TexCoord.s * 16.0, ex_TexCoord.t * 16.0, 0, 1);
  //gl_FragColor = vec4(ex_Color.x, ex_Color.y, ex_Color.z, 1);
  //gl_FragColor = vec4(1.0,1.0,1.0,1.0);
}