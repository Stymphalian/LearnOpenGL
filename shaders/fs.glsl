#version 330 core
out vec4 color;
in vec3 FragPos;
in vec3 NormCoord;
in vec2 TexCoord;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float specularStrength = 4.0;
uniform float ambientStrength = 0.1;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;

void main() {

  // normal and light directions
  vec3 norm = normalize(NormCoord);
  vec3 lightDir = normalize(lightPos - FragPos);

  // diffuse colouring
  float diffuse_magnitude = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diffuse_magnitude * lightColor;

  // ambient colouring
  vec3 ambient = ambientStrength * lightColor;

  // specular lighting
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  vec3 specular = specularStrength * spec * lightColor;
 
  vec3 result = (ambient + diffuse + specular) * objectColor;
  color = vec4(result, 1.0);

  //color = vec4(TexCoord.x, TexCoord.y, 0.0f, 1.0f);
  //color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
  //color = texture(ourTexture, TexCoord);
  //color = texture(texture_diffuse1, TexCoord);
  //color = mix(
      //texture(texture_diffuse1, TexCoord),
      //texture(texture_diffuse2, TexCoord), 0.2);
}
