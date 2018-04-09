#version 130

uniform sampler2D terrain;

in vec3 ex_Color;

void main(void) {
  vec4 color = texture2D(terrain, gl_TexCoord[0].st);
  color.a = 1.0;
  //if (color.a < 0.9) { discard; }
  gl_FragColor = color;// + vec4(ex_Color, 0.0);
  //gl_FragColor = vec4(gl_TexCoord[0].s * 16.0, gl_TexCoord[0].t * 16.0, 0, 1);
  //gl_FragColor = vec4(ex_Color.x, ex_Color.y, ex_Color.z, 1);
  //gl_FragColor = vec4(1.0,1.0,1.0,1.0);
}