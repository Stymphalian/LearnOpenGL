#include "object.h"

#include <iostream>
#include <memory>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "mesh.h"
#include "shader.h"

glm::vec3 Basis::x() const { return _x; }
glm::vec3 Basis::y() const { return _y; }
glm::vec3 Basis::z() const { return _z; }
Basis &Basis::set(glm::vec3 x, glm::vec3 y, glm::vec3 z) {
  _x = x;
  _y = y;
  _z = z;
  return *this;
}
Basis &Basis::setX(glm::vec3 x) {
  _x = x;
  return *this;
}
Basis &Basis::setY(glm::vec3 y) {
  _y = y;
  return *this;
}
Basis &Basis::setZ(glm::vec3 z) {
  _z = z;
  return *this;
}
Basis &Basis::reset() {
  _x = default_x;
  _y = default_y;
  _z = default_z;
  return *this;
}

Translator &Translator::move_to(glm::vec3 pos) {
  this->pos = pos;
  return *this;
}

Translator &Translator::translate(glm::vec3 delta) {
  this->pos += delta;
  return *this;
}

Translator &Translator::reset() {
  this->pos = glm::vec3(0, 0, 0);
  return *this;
}

glm::mat4 Translator::matrix(glm::mat4 model) {
  return glm::translate(model, pos);
}

const Basis Rotator::worldBasis = Basis();

Rotator &Rotator::rotate(glm::vec3 axis, float angle_deg) {
  glm::quat q = glm::angleAxis(glm::radians(angle_deg), glm::normalize(axis));
  _orientation = q * _orientation;
  update_vectors();
  return *this;
}

Rotator &Rotator::rotateX(float angle_deg) {
  return rotate(basis().x(), angle_deg);
}
Rotator &Rotator::rotateY(float angle_deg) {
  return rotate(basis().y(), angle_deg);
}
Rotator &Rotator::rotateZ(float angle_deg) {
  return rotate(basis().z(), angle_deg);
}
Rotator &Rotator::lookAt(glm::vec3 point) {
  return rotate(glm::normalize(point), 0);
}

Rotator &Rotator::reset() {
  _basis = Basis();
  _orientation = glm::angleAxis(glm::radians(0.0f), Basis::default_z);
  update_vectors();
  return *this;
}

glm::mat4 Rotator::matrix(glm::mat4 model = glm::mat4(1.0f)) {
  return model * glm::mat4_cast(_orientation);
  //model = glm::rotate(model, glm::radians(_roll), glm::vec3(0, 0, 1.0f));
  //model = glm::rotate(model, glm::radians(_pitch), glm::vec3(1.0f, 0, 0));
  //model = glm::rotate(model, glm::radians(_yaw), glm::vec3(0, 1.0f, 0));
}
glm::vec3 Rotator::right() const { return _basis.x(); }
glm::vec3 Rotator::front() const { return _basis.z(); }
glm::vec3 Rotator::up() const { return _basis.y(); }
float Rotator::pitch() const {
  return glm::degrees(glm::eulerAngles(_orientation).x);
}
float Rotator::yaw() const {
  return glm::degrees(glm::eulerAngles(_orientation).y);
}
float Rotator::roll() const {
  return glm::degrees(glm::eulerAngles(_orientation).z);
}

glm::vec3 Rotator::computeForwardVector(glm::quat q) const {
  const float x2 = 2.0f * q.x;
  const float y2 = 2.0f * q.y;
  const float z2 = 2.0f * q.z;
  const float x2w = x2 * q.w;
  const float y2w = y2 * q.w;
  const float x2x = x2 * q.x;
  const float z2x = z2 * q.x;
  const float y2y = y2 * q.y;
  const float z2y = z2 * q.y;
  return glm::vec3(z2x + y2w, z2y - x2w, 1.0f - (x2x + y2y));
}

glm::vec3 Rotator::computeRightVector(glm::quat q) const {
  const float y2 = 2.0f * q.y;
  const float z2 = 2.0f * q.z;
  const float y2w = y2 * q.w;
  const float z2w = z2 * q.w;
  const float y2x = y2 * q.x;
  const float z2x = z2 * q.x;
  const float y2y = y2 * q.y;
  const float z2z = z2 * q.z;
  return glm::vec3(1.0f - (y2y + z2z), y2x + z2w, z2x - y2w);
}

glm::vec3 Rotator::computeUpVector(glm::quat q) const {
  const float x2 = 2.0f * q.x;
  const float y2 = 2.0f * q.y;
  const float z2 = 2.0f * q.z;
  const float x2w = x2 * q.w;
  const float z2w = z2 * q.w;
  const float x2x = x2 * q.x;
  const float y2x = y2 * q.x;
  const float z2y = z2 * q.y;
  const float z2z = z2 * q.z;
  return glm::vec3(y2x - z2w, 1.0f - (x2x + z2z), z2y + x2w);
}

void Rotator::update_vectors() {
  glm::vec3 _right = computeRightVector(_orientation);
  glm::vec3 _up = computeUpVector(_orientation);
  glm::vec3 _front = computeForwardVector(_orientation);
  _basis.set(_right, _up, _front);
}

Basis Rotator::basis() { return _basis; }

Rotator &Rotator::setBasis(Basis b) {
  _basis = b;
  return *this;
}

Scaler &Scaler::scale(float percent) {
  scalar = glm::vec3(percent);
  return *this;
}

Scaler &Scaler::scale(float x, float y, float z) {
  scalar = glm::vec3(x, y, z);
  return *this;
}

glm::mat4 Scaler::matrix(glm::mat4 model) {
  model = glm::scale(model, scalar);
  return model;
}

void Object::draw(Shader &shader) {
  glm::mat4 modelMat = glm::mat4(1.0f);
  modelMat = position.matrix(modelMat);
  modelMat = rotation.matrix(modelMat);
  modelMat = scale.matrix(modelMat);
  // modelMat = scale.matrix(modelMat);

  shader.setMat4("model", modelMat);
  shader.setMat4("inv_model", glm::inverse(modelMat));
  model.draw(shader);
}
