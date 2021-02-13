#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <glm/gtc/constants.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include "shader.h"
#include "mesh.h"

class Translator {
public:
  glm::vec3 pos = glm::vec3(0,0,0);

  Translator& move_to(glm::vec3 pos);
  Translator& translate(glm::vec3 delta); 
  glm::mat4 matrix(glm::mat4 model); 
};

class Rotator {
public:
  float pitch = 0.0f;;
  float yaw = 0.0f;
  float roll = 0.0f;

  Rotator& rotate(glm::vec3 axis, float angle_deg); 

  Rotator& rotateX(float angle_deg); 
  Rotator& rotateY(float angle_deg); 
  Rotator& rotateZ(float angle_deg); 
  Rotator& reset(); 
  glm::mat4 matrix(glm::mat4 model); 
};

class Scaler {
public:
  glm::vec3 scalar = glm::vec3(1.0f);

  Scaler& scale(float percent); 
  glm::mat4 matrix(glm::mat4 model); 
};

class Object {
public:
  Mesh& mesh;
  Translator position;
  Rotator rotation;
  Scaler scale;

  Object(Mesh& mesh): mesh(mesh) {}

  void draw(Shader& shader);
};
