#include "camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"


Camera::Camera() {
  translator.reset();
  rotator.reset();
  translator.pos.z = 3.0f;
}

void Camera::process_input(Camera::Direction dir, float delta_secs) {
  float velocity = movement_speed * delta_secs;
  if (dir == Direction::FORWARD) { translator.pos += rotator.front*velocity; }
  if (dir == Direction::BACKWARD) { translator.pos -= rotator.front*velocity; }
  if (dir == Direction::RIGHT) { translator.pos -= rotator.right*velocity; }
  if (dir == Direction::LEFT) { translator.pos += rotator.right*velocity; }
  if (dir == Direction::UP) { translator.pos += rotator.world_up*velocity; }
  if (dir == Direction::DOWN) { translator.pos -= rotator.world_up*velocity; }
}

void Camera::process_rotate(Camera::Rotate dir, float delta_secs) {
  if (dir == YAW_RIGHT) {
    rotator.relativeRotateY(rotation_speed * delta_secs);
  }
  if (dir == YAW_LEFT) {
    rotator.relativeRotateY(-rotation_speed * delta_secs);
  }
  if (dir == PITCH_UP) {
    rotator.relativeRotateX(rotation_speed * delta_secs);
  }
  if (dir == PITCH_DOWN) {
    rotator.relativeRotateX(-rotation_speed * delta_secs);
  }
  if (dir == ROLL_CLOCKWISE) {
    rotator.relativeRotateZ(-rotation_speed * delta_secs);
  }
  if (dir == ROLL_COUNTER_CLOCKWISE) {
    rotator.relativeRotateZ(rotation_speed * delta_secs);
  }
}

void Camera::process_mouse(float x_offset, float y_offset) {
  return;
  x_offset *= rotation_senstivity;
  y_offset *= rotation_senstivity;

  rotator.yaw += x_offset;
  rotator.pitch += y_offset;
  rotator.update_vectors();
}

void Camera::process_zoom() {
}

void Camera::use(float aspect_ratio, Shader* shader) {
  //glm::mat4 mvp = projection * view  * model;
  shader->setMat4("projection", projection(aspect_ratio));
  shader->setMat4("view", view());
  //shader->setMat4("inv_projection", projection(aspect_ratio));
  shader->setMat4("inv_view", glm::inverse(view()));

  shader->set3Float("viewPos", translator.pos);
}

glm::mat4 Camera::projection(float aspect_ratio) const {
  return glm::perspective(glm::radians(fov), aspect_ratio, near, far);
}

glm::mat4 Camera::view() const {
  return glm::lookAt(translator.pos, 
                     translator.pos+rotator.front,
                     rotator.up);
}
