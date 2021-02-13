#include "object.h"

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

Translator& Translator::move_to(glm::vec3 pos) {
  this->pos = pos;
  return *this;
}

Translator& Translator::translate(glm::vec3 delta) { 
  this->pos += delta;
  return *this;
}

glm::mat4 Translator::matrix(glm::mat4 model) {
  return glm::translate(model, pos);
}

Rotator& Rotator::rotate(glm::vec3 axis, float angle_deg) {
  glm::quat q = glm::angleAxis(glm::radians(angle_deg), glm::normalize(axis));
  glm::vec3 e = glm::eulerAngles(q);
  pitch = glm::degrees(e.x);
  yaw = glm::degrees(e.y);
  roll = glm::degrees(e.z);
  return *this;
}

Rotator& Rotator::rotateX(float angle_deg) {
  pitch = angle_deg;
  if (pitch <= -89.0) { pitch = -89.0; } 
  if (pitch >= 89.0) { pitch = 89.0; } 
  return *this;
}
Rotator& Rotator::rotateY(float angle_deg) { 
  yaw = angle_deg;
  return *this;
}
Rotator& Rotator::rotateZ(float angle_deg) { 
  roll = angle_deg; 
  return *this;
}

Rotator& Rotator::reset() {
  pitch = 0.0f;
  yaw = 0.0f;
  roll = 0.0f;
  return *this;
}

glm::mat4 Rotator::matrix(glm::mat4 model) {
  model = glm::rotate(model, glm::radians(pitch), glm::vec3(1.0f, 0, 0));
  model = glm::rotate(model, glm::radians(yaw), glm::vec3(0, 1.0f, 0));
  model = glm::rotate(model, glm::radians(roll), glm::vec3(0, 0, -1.0f));
  return model;
}

Scaler& Scaler::scale(float percent) {
  scalar = glm::vec3(percent);
  return *this;
}

glm::mat4 Scaler::matrix(glm::mat4 model) {
  model = glm::scale(model, scalar);
  return model;
}

void Object::draw(Shader& shader) {
  glm::mat4 model = glm::mat4(1.0f);
  model = position.matrix(model);
  model = rotation.matrix(model);
  model = scale.matrix(model);

  shader.setMat4("model", model);
  mesh.draw(shader);
}