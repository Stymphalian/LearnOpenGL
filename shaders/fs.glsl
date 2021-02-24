#version 330 core
out vec4 color;
in vec3 FragPos;
in vec3 NormCoord;
in vec2 TexCoord;

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  sampler2D emission;
  float shininess;
};

struct Light {
  vec3 position;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;

  bool is_directional;
  
  bool is_spotlight;
  vec3 direction;
  float innerCutoff;
  float outerCutoff;
};

uniform vec3 viewPos;
uniform Light light;
uniform Material material;
//uniform sampler2D texture_diffuse1;
//uniform sampler2D texture_diffuse2;

void main() {

  // normal and light directions
  vec3 norm = normalize(NormCoord);
  vec3 lightDir = normalize(light.position - FragPos);
  if (light.is_directional) {
    lightDir = normalize(light.position);
  }

  // ambient colouring
  vec3 diffuse_texture = vec3(texture(material.diffuse, TexCoord));
  vec3 ambient = light.ambient * diffuse_texture;

  // diffuse colouring
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = light.diffuse * (diff *  diffuse_texture);

  // specular lighting
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular_texture = vec3(texture(material.specular, TexCoord));
  vec3 specular = light.specular * (spec * specular_texture);

  // emission
  vec3 emission = vec3(texture(material.emission, TexCoord));

  // attenuation
  float dist = length(light.position - FragPos);
  float attenuation = 1.0 / (light.constant + light.linear * dist 
      + light.quadratic * dist *dist);
  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;

  // spot light calculation
  if (light.is_spotlight) {
    vec3 spotDir = normalize(-light.direction);
    float lightCosTheta = dot(lightDir, spotDir);

    // apply lighting thresholds if it is the outer range of the light cone
    if (lightCosTheta < light.outerCutoff) {
      diffuse = vec3(0,0,0);
      specular = vec3(0,0,0);
    } else if (lightCosTheta < light.innerCutoff) {
      // must interpolate because we are in the inner outer cone range
      float range = lightCosTheta - light.outerCutoff;
      float weight = abs(light.innerCutoff  - light.outerCutoff);
      float intensity = clamp(range / weight, 0.0, 1.0);;
      diffuse *= intensity;
      specular *= intensity;
    } else {
      // use full illumination
    }
  }

  vec3 result = (ambient + diffuse + specular);
  //vec3 result = (emission + ambient + diffuse + specular);
  color = vec4(result, 1.0);

  //color = vec4(TexCoord.x, TexCoord.y, 0.0f, 1.0f);
  //color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
  //color = texture(ourTexture, TexCoord);
  //color = texture(texture_diffuse1, TexCoord);
  //color = mix(
      //texture(texture_diffuse1, TexCoord),
      //texture(texture_diffuse2, TexCoord), 0.2);
}
