#include "camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

void Camera::process_input(Camera::Direction dir, float delta_secs) {
  float velocity = movement_speed * delta_secs;
  if (dir == Direction::FORWARD) { pos += front*velocity; }
  if (dir == Direction::BACKWARD) { pos -= front*velocity; }
  if (dir == Direction::RIGHT) { pos += right*velocity; }
  if (dir == Direction::LEFT) { pos -= right*velocity; }
  if (dir == Direction::UP) { pos += world_up*velocity; }
  if (dir == Direction::DOWN) { pos -= world_up*velocity; }
}

void Camera::rotate(Camera::Rotate dir, float delta_secs) {
  if (dir == YAW_RIGHT) {yaw += rotation_speed*delta_secs;}
  if (dir == YAW_LEFT) {yaw -= rotation_speed*delta_secs;}
  if (dir == PITCH_UP) {pitch += rotation_speed*delta_secs;}
  if (dir == PITCH_DOWN) {pitch -= rotation_speed*delta_secs;}
  clamp_pitch();
  update_camera_vectors();
}

void Camera::clamp_pitch() {
  if (pitch <= -89.0) { pitch = -89.0;}
  if (pitch >= 89.0) { pitch = 89.0;}
}

void Camera::process_mouse(float x_offset, float y_offset) {
  x_offset *= rotation_senstivity;
  y_offset *= rotation_senstivity;

  yaw += x_offset;
  pitch += y_offset;
  clamp_pitch();
  update_camera_vectors();
}

void Camera::process_zoom() {
}

void Camera::use(float aspect_ratio, Shader* shader) {
  //glm::mat4 mvp = projection * view  * model;
  shader->setMat4("projection", projection(aspect_ratio));
  shader->setMat4("view", view());
}

void Camera::update_camera_vectors() {
  glm::vec3 f;
  f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  f.y = sin(glm::radians(pitch));
  f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  front = glm::normalize(f);

  // also re-calculate the Right and Up vector
  right = glm::normalize(glm::cross(front, world_up)); 
  up = glm::normalize(glm::cross(right, front));
}

glm::mat4 Camera::projection(float aspect_ratio) const {
  return glm::perspective(glm::radians(fov), aspect_ratio, near, far);
}

glm::mat4 Camera::view() const {
  return glm::lookAt(pos, pos+front, world_up);
}
