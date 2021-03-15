#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>
#include <vector>

#include "mesh.h"
#include "model.h"
#include "shader.h"

class Basis {
public:
  static constexpr glm::vec3 default_x = glm::vec3(1.0f, 0, 0);
  static constexpr glm::vec3 default_y = glm::vec3(0, 1.0f, 0);
  static constexpr glm::vec3 default_z = glm::vec3(0, 0, 1.0f);

  glm::vec3 x() const;
  glm::vec3 y() const;
  glm::vec3 z() const;

  Basis& set(glm::vec3 x, glm::vec3 y, glm::vec3 z);
  Basis& setX(glm::vec3 x);
  Basis& setY(glm::vec3 x);
  Basis& setZ(glm::vec3 x);
  Basis& reset();

  Basis(): _x(default_x), _y(default_y), _z(default_z) {}
  Basis(glm::vec3 x, glm::vec3 y, glm::vec3 z): _x(x), _y(y), _z(z) {}
private:
  glm::vec3 _x = default_x;
  glm::vec3 _y = default_y;
  glm::vec3 _z = default_z;
};

class Translator {
public:
  glm::vec3 pos = glm::vec3(0, 0, 0);

  Translator &move_to(glm::vec3 pos);
  Translator &translate(glm::vec3 delta);
  Translator &reset();
  glm::mat4 matrix(glm::mat4 model);

  friend std::ostream &operator<<(std::ostream &os, const Translator &t) {
    os << "(" << t.pos.x << "," << t.pos.y << "," << t.pos.z << ")";
    return os;
  }
};

class Rotator {
public:
  static const Basis worldBasis;

  Rotator &lookAt(glm::vec3 point);
  Rotator &rotate(glm::vec3 axis, float angle_deg);
  Rotator &rotateX(float angle_deg);
  Rotator &rotateY(float angle_deg);
  Rotator &rotateZ(float angle_deg);

  Rotator &reset();
  void update_vectors();

  glm::mat4 matrix(glm::mat4 model);

  Basis basis();
  Rotator& setBasis(Basis b);
  glm::vec3 right() const;
  glm::vec3 front() const;
  glm::vec3 up() const;
  float pitch() const;
  float yaw() const;
  float roll() const;

  glm::quat orientation() const;
  Rotator& setOrientation();

  friend std::ostream &operator<<(std::ostream &os, const Rotator &t) {
    os << "{";
    os << "(r=" << string_format("%1.2f,%1.2f,%1.2f),", t.right().x, t.right().y, t.right().z);
    os << "(u=" << string_format("%1.2f,%1.2f,%1.2f),", t.up().x, t.up().y, t.up().z);
    os << "(f=" << string_format("%1.2f,%1.2f,%1.2f),", t.front().x, t.front().y, t.front().z);
    os << string_format("(p=%3.2f,y=%3.2f,r=%3.2f)", t.pitch(), t.yaw(), t.roll());
    os << "}";
    return os;
  }


protected:
  Basis _basis;
  glm::quat _orientation = glm::angleAxis(glm::radians(0.0f), glm::vec3(0,0,1));

  glm::vec3 computeRightVector(glm::quat q) const;
  glm::vec3 computeUpVector(glm::quat q) const;
  glm::vec3 computeForwardVector(glm::quat q) const;
};

class Scaler {
public:
  glm::vec3 scalar = glm::vec3(1.0f);

  Scaler &scale(float percent);
  Scaler &scale(float x, float y, float z);
  glm::mat4 matrix(glm::mat4 model);
};

class Object {
public:
  Model &model;
  Translator position;
  Rotator rotation;
  Scaler scale;

  Object(Model &model) : model(model) {}

  void draw(Shader &shader);
};
