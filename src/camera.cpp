#include "camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"


Camera::Camera() { 
  reset();
}
Camera::Camera(float movement_speed, float rotation_speed,
               float rotation_sensitivity)
    : movement_speed(movement_speed), rotation_speed(rotation_speed),
      rotation_senstivity(rotation_sensitivity) {
  reset();
}

void Camera::process_input(Camera::Direction dir, float delta_secs) {
  float velocity = movement_speed * delta_secs;
  if (dir == Direction::FORWARD) { translator.pos += rotator.front()*velocity; }
  if (dir == Direction::BACKWARD) { translator.pos -= rotator.front()*velocity; }
  if (dir == Direction::RIGHT) { translator.pos -= rotator.right()*velocity; }
  if (dir == Direction::LEFT) { translator.pos += rotator.right()*velocity; }
  if (dir == Direction::UP) { translator.pos += Rotator::worldBasis.y()*velocity; }
  if (dir == Direction::DOWN) { translator.pos -= Rotator::worldBasis.y()*velocity; }
}


// This gets confusing. We are rotating in world space using RH system
// But when we are the camera, we need to induce the opposite rotation
// in-order for the movement to feel natural
void Camera::process_rotate(Camera::Rotate dir, float delta_secs) {
  if (dir == YAW_RIGHT) {
    rotator.rotateY(-rotation_speed * delta_secs);
  }
  if (dir == YAW_LEFT) {
    rotator.rotateY(rotation_speed * delta_secs);
  }
  if (dir == PITCH_UP) {
    rotator.rotateX(-rotation_speed * delta_secs);
  }
  if (dir == PITCH_DOWN) {
    rotator.rotateX(rotation_speed * delta_secs);
  }
  if (dir == ROLL_CLOCKWISE) {
    rotator.rotateZ(-rotation_speed * delta_secs);
  }
  if (dir == ROLL_COUNTER_CLOCKWISE) {
    rotator.rotateZ(rotation_speed * delta_secs);
  }
}

void Camera::process_mouse(float x_offset, float y_offset) {
  return;
}

void Camera::process_zoom() {
}

void Camera::reset() {
  translator.reset();
  translator.pos.z = 3.0f;
  rotator.reset();
  rotator.rotateY(180.0f);
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
                     translator.pos+rotator.front(),
                     rotator.up());
}
