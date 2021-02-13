#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

class Camera {
public:
  enum Direction {
    FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN
  };
  enum Rotate {
    YAW_RIGHT, YAW_LEFT, PITCH_UP, PITCH_DOWN
  };

  glm::vec3 pos = glm::vec3(0,0,3.0f);
  glm::vec3 front = glm::vec3(0, 0, -1.0f);
  glm::vec3 right = glm::vec3(1.0f, 0, 0);
  glm::vec3 up = glm::vec3(0, 1.0f, 0);
  glm::vec3 world_up = glm::vec3(0, 1.0f, 0);

  float yaw = -90.0f;
  float pitch = 0.0f;

  float fov = 45.0f;
  float near = 0.1f;
  float far = 100.0f;

  float movement_speed = 2.5f;
  float rotation_speed = 30.0f;
  float rotation_senstivity = 1.0f;

  void process_input(Direction dir, float delta_secs); 
  void rotate(Rotate dir, float delta_secs); 
  void clamp_pitch(); 
  void process_mouse(float x_offset, float y_offset); 
  void process_zoom();
  void use(float aspect_ratio, Shader* shader); 

  void update_camera_vectors(); 

  glm::mat4 projection(float aspect_ratio) const; 
  glm::mat4 view() const; 
};
