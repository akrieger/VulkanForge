#version 150

uniform mat4 in_MVP;
uniform mat4 in_MV;
uniform mat4 in_P;

uniform int chunk_x;
uniform int chunk_z;

in vec3 in_Position;
in vec2 in_Tex;

out vec3 ex_Color;
out vec2 ex_TexCoord;

void main(void) {
  // Multiply the mvp matrix by the vertex to obtain our final vertex position
  vec4 vertex = vec4(in_Position, 1);
  ex_Color = vec3(vertex.x, vertex.y, vertex.z);
  ex_TexCoord = in_Tex;
  vertex.x += 32.0 * chunk_x;
  vertex.z += 32.0 * chunk_z;
  gl_Position = in_MVP * vertex;
}