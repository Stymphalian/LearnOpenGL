#version 330 core
out vec4 color;
in vec3 FragPos;
in vec3 NormCoord;
in vec2 TexCoord;

#define NUMBER_OF_LIGHTS 5
#define NUMBER_OF_TEXTURES 5

struct Material {
  sampler2D diffuse_0;
  sampler2D diffuse_1;
  sampler2D diffuse_2;
  sampler2D diffuse_3;
  sampler2D diffuse_4;

  sampler2D specular_0;
  sampler2D specular_1;
  sampler2D specular_2;
  sampler2D specular_3;
  sampler2D specular_4;

  sampler2D emission_0;
  sampler2D emission_1;
  sampler2D emission_2;
  sampler2D emission_3;
  sampler2D emission_4;

  float shininess;

  int numDiffuse;
  int numSpecular;
  int numEmission;
};

struct MaterialTexture {
  vec3 diffuse[NUMBER_OF_TEXTURES];
  vec3 specular[NUMBER_OF_TEXTURES];
  vec3 emission[NUMBER_OF_TEXTURES];
  float shininess;

  int numDiffuse;
  int numSpecular;
  int numEmission;
};

struct LightContext {
  vec3 viewDir;
  vec3 normal;
  MaterialTexture m;
};

struct LightColor {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct DirectionalLight {
  vec3 direction;
  LightColor light;
};

struct Attenuation {
  float constant;
  float linear;
  float quadratic;
};

struct PointLight {
  vec3 position;
  LightColor light;
  Attenuation attenuation;
};

struct SpotLight {
  PointLight point;
  vec3 direction;
  float innerCutoff;
  float outerCutoff;
};

// uniform Light light;
uniform vec3 viewPos;
uniform Material material;

uniform DirectionalLight directionalLights[NUMBER_OF_LIGHTS];
uniform PointLight pointLights[NUMBER_OF_LIGHTS];
uniform SpotLight spotLights[NUMBER_OF_LIGHTS];
uniform int numDirLights;
uniform int numPointLights;
uniform int numSpotLights;

MaterialTexture getMaterialTexture(Material m);
LightColor calculateLightColor(LightColor l, LightContext c, vec3 lightDir);
LightColor applyLightAttenuation(LightColor result, vec3 lightPos,
                                 Attenuation attenuation);
LightColor calculateDirectionalLight(DirectionalLight l, LightContext c);
LightColor calculatePointLight(PointLight l, LightContext c);
LightColor calculateSpotLight(SpotLight l, LightContext c);

void main() {
  LightContext context;
  context.normal = normalize(NormCoord);
  context.viewDir = normalize(viewPos - FragPos);
  context.m = getMaterialTexture(material);

  vec3 result = vec3(0.0f);

  // calculate all directional lights
  for (int i = 0; i < min(numDirLights, NUMBER_OF_LIGHTS); i++) {
    LightColor r = calculateDirectionalLight(directionalLights[i], context);
    result += (r.ambient + r.diffuse + r.specular);
  }
  // calculate all point lights
  for (int i = 0; i < min(numPointLights, NUMBER_OF_LIGHTS); i++) {
    LightColor r = calculatePointLight(pointLights[i], context);
    result += (r.ambient + r.diffuse + r.specular);
  }
  // calculate all spot lights
  for (int i = 0; i < min(numSpotLights, NUMBER_OF_LIGHTS); i++) {
    LightColor r = calculateSpotLight(spotLights[i], context);
    result += (r.ambient + r.diffuse + r.specular);
  }
  // calculate emission light from material
  //for (int i = 0; i < min(context.m.numEmission, NUMBER_OF_TEXTURES); i++) {
    //result += context.m.emission[i];
  //}

  color = vec4(result, 1.0);
}

MaterialTexture getMaterialTexture(Material m) {
  MaterialTexture t;
  if (m.numDiffuse >= 1) { t.diffuse[0] = vec3(texture(m.diffuse_0, TexCoord)); }
  if (m.numDiffuse >= 2) { t.diffuse[1] = vec3(texture(m.diffuse_1, TexCoord)); }
  if (m.numDiffuse >= 3) { t.diffuse[2] = vec3(texture(m.diffuse_2, TexCoord)); }
  if (m.numDiffuse >= 4) { t.diffuse[3] = vec3(texture(m.diffuse_3, TexCoord)); }
  if (m.numDiffuse >= 5) { t.diffuse[4] = vec3(texture(m.diffuse_4, TexCoord)); }

  if (m.numSpecular >= 1) { t.specular[0] = vec3(texture(m.specular_0, TexCoord)); }
  if (m.numSpecular >= 2) { t.specular[1] = vec3(texture(m.specular_1, TexCoord)); }
  if (m.numSpecular >= 3) { t.specular[2] = vec3(texture(m.specular_2, TexCoord)); }
  if (m.numSpecular >= 4) { t.specular[3] = vec3(texture(m.specular_3, TexCoord)); }
  if (m.numSpecular >= 5) { t.specular[4] = vec3(texture(m.specular_4, TexCoord)); }

  if (m.numEmission >= 1) { t.emission[0] = vec3(texture(m.emission_0, TexCoord)); }
  if (m.numEmission >= 2) { t.emission[1] = vec3(texture(m.emission_1, TexCoord)); }
  if (m.numEmission >= 3) { t.emission[2] = vec3(texture(m.emission_2, TexCoord)); }
  if (m.numEmission >= 4) { t.emission[3] = vec3(texture(m.emission_3, TexCoord)); }
  if (m.numEmission >= 5) { t.emission[4] = vec3(texture(m.emission_4, TexCoord)); }

  //t.diffuse = vec3(texture(m.diffuse, TexCoord));
  //t.specular = vec3(texture(m.specular, TexCoord));
  //t.emission = vec3(texture(m.emission, TexCoord));

  t.shininess = m.shininess;

  t.numDiffuse = m.numDiffuse;
  t.numSpecular = m.numSpecular;
  t.numEmission = m.numEmission;
  return t;
}

LightColor calculateLightColor(LightColor l, LightContext c, vec3 lightDir) {
  LightColor result;
  result.ambient = vec3(0,0,0);
  result.diffuse = vec3(0,0,0);
  result.specular = vec3(0,0,0);

  // ambient colouring
  for (int i = 0; i < c.m.numDiffuse; i++) {
    result.ambient += (l.ambient * c.m.diffuse[i]);
  }

  // diffuse colouring
  float diff = max(dot(c.normal, lightDir), 0.0);
  for (int i = 0; i < c.m.numDiffuse; i++) {
    result.diffuse += (l.diffuse * (diff * c.m.diffuse[i]));
  }

  // specular lighting
  vec3 reflectDir = reflect(-lightDir, c.normal);
  float spec = pow(max(dot(c.viewDir, reflectDir), 0.0), c.m.shininess);
  for (int i = 0; i < c.m.numSpecular; i++) {
    result.specular += (l.specular * (spec * c.m.specular[i]));
  }

  return result;
}

LightColor applyLightAttenuation(LightColor result, vec3 lightPos,
                                 Attenuation attenuation) {
  // attenuation
  float dist = length(lightPos - FragPos);
  float att = 1.0 / (attenuation.constant + attenuation.linear * dist +
                     attenuation.quadratic * dist * dist);

  result.ambient *= att;
  result.diffuse *= att;
  result.specular *= att;
  return result;
}

LightColor calculateDirectionalLight(DirectionalLight l, LightContext c) {
  //vec3 lightDir = normalize(l.direction);
  vec3 lightDir = normalize(-l.direction);
  LightColor result = calculateLightColor(l.light, c, lightDir);
  return result;
}

LightColor calculatePointLight(PointLight l, LightContext c) {
  vec3 lightDir = normalize(l.position - FragPos);

  // Get the normal light color
  LightColor result = calculateLightColor(l.light, c, lightDir);

  // Apply attenuation
  result = applyLightAttenuation(result, l.position, l.attenuation);
  return result;
}

LightColor calculateSpotLight(SpotLight light, LightContext c) {
  LightColor result = calculatePointLight(light.point, c);

  // apply lighting thresholds if it is the outer range of the light cone
  vec3 lightDir = normalize(light.point.position - FragPos);
  vec3 spotDir = normalize(-light.direction);
  float lightCosTheta = dot(lightDir, spotDir);
  if (lightCosTheta < light.outerCutoff) {
    // We are outisde the cone, just black it out
    result.diffuse = vec3(0, 0, 0);
    result.specular = vec3(0, 0, 0);
  } else if (lightCosTheta < light.innerCutoff) {
    // must interpolate because we are in the inner outer cone range
    float range = lightCosTheta - light.outerCutoff;
    float weight = abs(light.innerCutoff - light.outerCutoff);
    float intensity = clamp(range / weight, 0.0, 1.0);
    result.diffuse *= intensity;
    result.specular *= intensity;
  } else {
    // use full illumination
  }
  return result;
};
