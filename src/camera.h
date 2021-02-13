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

  void process_input(Direction dir, float delta_secs) {
    float velocity = movement_speed * delta_secs;
    if (dir == Direction::FORWARD) { pos += front*velocity; }
    if (dir == Direction::BACKWARD) { pos -= front*velocity; }
    if (dir == Direction::RIGHT) { pos += right*velocity; }
    if (dir == Direction::LEFT) { pos -= right*velocity; }
    if (dir == Direction::UP) { pos += world_up*velocity; }
    if (dir == Direction::DOWN) { pos -= world_up*velocity; }
  }

  void rotate(Rotate dir, float delta_secs) {
    if (dir == YAW_RIGHT) {yaw += rotation_speed*delta_secs;}
    if (dir == YAW_LEFT) {yaw -= rotation_speed*delta_secs;}
    if (dir == PITCH_UP) {pitch += rotation_speed*delta_secs;}
    if (dir == PITCH_DOWN) {pitch -= rotation_speed*delta_secs;}
    clamp_pitch();
    update_camera_vectors();
  }

  void clamp_pitch() {
    if (pitch <= -89.0) { pitch = -89.0;}
    if (pitch >= 89.0) { pitch = 89.0;}
  }

  void process_mouse(float x_offset, float y_offset) {
    x_offset *= rotation_senstivity;
    y_offset *= rotation_senstivity;

    yaw += x_offset;
    pitch += y_offset;
    clamp_pitch();
    update_camera_vectors();
  }

  void process_zoom() {
  }

  void use(float aspect_ratio, Shader* shader) {
    //glm::mat4 mvp = projection * view  * model;
    shader->setMat4("projection", projection(aspect_ratio));
    shader->setMat4("view", view());
  }

  void update_camera_vectors() {
    glm::vec3 f;
    f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    f.y = sin(glm::radians(pitch));
    f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(f);

    // also re-calculate the Right and Up vector
    right = glm::normalize(glm::cross(front, world_up)); 
    up = glm::normalize(glm::cross(right, front));
  }

  glm::mat4 projection(float aspect_ratio) const {
    return glm::perspective(glm::radians(fov), aspect_ratio, near, far);
  }

  glm::mat4 view() const {
    return glm::lookAt(pos, pos+front, world_up);
  }
};
