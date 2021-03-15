#pragma once

#include <glm/glm.hpp>

//#include "mesh.h"
#include "model.h"
#include "object.h"
#include "shader.h"

struct Light : public Object {
public:
  enum LIGHT_TYPE_E { POINT, SPOT, DIRECTIONAL };
  LIGHT_TYPE_E type = POINT;

  // The color emitted by the light
  glm::vec3 ambient = glm::vec3(0.1f, 0.1f, 0.1f);
  glm::vec3 diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
  glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);

  // Used for Point and Spot lights
  //glm::vec3 _position = glm::vec3(0,0,0);

  // Used for spot and directional lights
  //glm::vec3 _direction = glm::vec3(0,0,1.0f);

  // attenuation. Not used for DIRECTIONAL lights
  float constant = 1.0f;
  float linear = 0.09f;
  float quadratic = 0.032f;

  // Used only for spot lights
  float innerCutoff = glm::cos(glm::radians(12.5f));
  float outerCutoff = glm::cos(glm::radians(17.5f));

  Light(Model& model, LIGHT_TYPE_E type=POINT): Object(model), type(type) {}
  void use(Shader *shader, int lightIndex);

  glm::vec3 getPosition() { return position.pos;}
  glm::vec3 getDirection() { return rotation.front();}

protected:
  void useAsPoint(Shader *shader, int lightIndex);
  void useAsDirectional(Shader *shader, int lightIndex);
  void useAsSpot(Shader *shader, int lightIndex);
};
