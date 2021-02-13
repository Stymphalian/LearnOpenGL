#version 330 core
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 Texture;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 NormCoord;
out vec2 TexCoord;
out ViewOut_t { vec3 Normal; } vs_out;

void main() {
  TexCoord = Texture;
  NormCoord = Normal;
  gl_Position = view * model * vec4(Position, 1.0f);

  mat3 normalMatrix = mat3(transpose(inverse(view * model)));
  vs_out.Normal = normalize(normalMatrix * Normal);
}
