#version 330 core
out vec4 color;
in vec3 FragPos;
in vec3 NormCoord;
in vec2 TexCoord;

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

struct Light {
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  //float shininess;
};

//uniform vec3 lightColor;
//uniform vec3 lightPos;

//uniform vec3 objectColor;
//uniform float specularStrength = 4.0;
//uniform float ambientStrength = 0.1;

uniform vec3 viewPos;
uniform Light light;
uniform Material material;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;

void main() {

  // normal and light directions
  vec3 norm = normalize(NormCoord);
  vec3 lightDir = normalize(light.position - FragPos);

  // ambient colouring
  vec3 ambient = light.ambient * material.ambient;

  // diffuse colouring
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = light.diffuse * (material.diffuse * diff);

  // specular lighting
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular * (spec * material.specular);
 
  vec3 result = (ambient + diffuse + specular);
  color = vec4(result, 1.0);

  //color = vec4(TexCoord.x, TexCoord.y, 0.0f, 1.0f);
  //color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
  //color = texture(ourTexture, TexCoord);
  //color = texture(texture_diffuse1, TexCoord);
  //color = mix(
      //texture(texture_diffuse1, TexCoord),
      //texture(texture_diffuse2, TexCoord), 0.2);
}
