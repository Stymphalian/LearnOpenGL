#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "object.h"

class Camera {
public:
  Translator translator;
  Rotator rotator;

  enum Direction {
    FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN
  };
  enum Rotate {
    YAW_RIGHT, YAW_LEFT, PITCH_UP, PITCH_DOWN,
    ROLL_CLOCKWISE, ROLL_COUNTER_CLOCKWISE
  };

  float fov = 45.0f;
  float near = 0.1f;
  float far = 100.0f;

  float movement_speed = 2.5f;
  float rotation_speed = 30.0f;
  //float rotation_speed = 1.0f;
  float rotation_senstivity = 1.0f;

  Camera();

  void process_input(Direction dir, float delta_secs); 
  void process_rotate(Rotate dir, float delta_secs); 
  void process_mouse(float x_offset, float y_offset); 
  void process_zoom();
  void reset();
  void use(float aspect_ratio, Shader *shader);

  //void update_camera_vectors(); 

  glm::mat4 projection(float aspect_ratio) const; 
  glm::mat4 view() const; 

  friend std::ostream& operator<< (std::ostream& os, const Camera& c) {
      os << c.translator << "," << c.rotator;
      return os;
  }
};
