#version 330 core
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 Texture;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
//uniform mat4 inv_projection;
uniform mat4 inv_view;
uniform mat4 inv_model;

out vec3 FragPos;
out vec3 NormCoord;
out vec2 TexCoord;

void main() {
  TexCoord = Texture;

  // caluculate to allow for ambient, diffuse and specular lighting
  //mat3 normalMatrix = mat3(transpose(inverse(model)));
  mat3 normalMatrix = mat3(transpose(inv_model));
  NormCoord = normalize(normalMatrix * Normal);
  FragPos = vec3(model * vec4(Position, 1.0f));

  gl_Position = projection * view * model * vec4(Position, 1.0f);

}
