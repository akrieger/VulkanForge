#version 130

in mat4x4 in_MVP;
in vec3 in_Position;
in vec2 in_Tex;

uniform int chunk_x;
uniform int chunk_z;

out vec3 ex_Color;

void main(void) {
  // Multiply the mvp matrix by the vertex to obtain our final vertex position
  vec4 vertex = vec4(in_Position, 1);
  ex_Color = vec3(vertex.x, vertex.y, vertex.z);
  gl_TexCoord[0] = vec4(in_Tex, 0, 0);
  vertex.x += 16.0 * chunk_x;
  vertex.z += 16.0 * chunk_z;
  gl_Position = gl_ModelViewProjectionMatrix * vertex;
}