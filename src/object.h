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
  Translator& reset();
  glm::mat4 matrix(glm::mat4 model); 

  friend std::ostream& operator<< (std::ostream& os, const Translator& t) {
    os << "(" << t.pos.x << "," << t.pos.y << "," << t.pos.z << ")";
    return os;
  }
};

class Rotator {
public:
  static constexpr glm::vec3 default_right = glm::vec3(-1.0f, 0,0);
  static constexpr glm::vec3 default_up    = glm::vec3(0, 1.0f, 0);
  static constexpr glm::vec3 default_front = glm::vec3(0, 0, -1.0f);
  static constexpr glm::vec3 world_up = glm::vec3(0.0f ,1.0f, 0.0f);

  float pitch = 0.0f;;
  float yaw = 0.0f;
  float roll = 0.0f;
  glm::vec3 right = default_right;
  glm::vec3 up    = default_up;
  glm::vec3 front = default_front;

  Rotator& rotate(glm::vec3 axis, float angle_deg); 
  Rotator& rotateX(float angle_deg); 
  Rotator& rotateY(float angle_deg); 
  Rotator& rotateZ(float angle_deg);
  Rotator& relativeRotateX(float angle_deg); 
  Rotator& relativeRotateY(float angle_deg); 
  Rotator& relativeRotateZ(float angle_deg);


  Rotator& reset();
  void update_vectors();

  glm::mat4 matrix(glm::mat4 model); 

  friend std::ostream& operator<< (std::ostream& os, const Rotator& t) {
    os << "{";
    os << "(r=" << t.right.x << "," << t.right.y << "," << t.right.z << "),";
    os << "(u=" << t.up.x << "," << t.up.y << "," << t.up.z << ").";
    os << "(f=" << t.front.x << "," << t.up.y << "," << t.up.z << "),";
    os << "(p=" << t.pitch << ",y=" << t.yaw << ",r=" << t.roll << ")";
    os << "}";
    return os;
  }

protected:
  void clamp_pitch();
};

class Scaler {
public:
  glm::vec3 scalar = glm::vec3(1.0f);

  Scaler& scale(float percent); 
  Scaler& scale(float x, float y, float z); 
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
