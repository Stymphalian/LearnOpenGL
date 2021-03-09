#pragma once

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include "stb_image.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <experimental/filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

template <typename... Args>
std::string string_format(const std::string &format, Args... args) {
  int size =
      snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
  if (size <= 0) {
    throw std::runtime_error("Error during formatting.");
  }
  std::unique_ptr<char[]> buf(new char[size]);
  snprintf(buf.get(), size, format.c_str(), args...);
  return std::string(buf.get(),
                     buf.get() + size - 1); // We don't want the '\0' inside
}

unsigned int load_texture(std::string image_filepath);

struct Util {
public:
  static void print_mat4(glm::mat4 m);
  static void print_quat(glm::quat q);
  static void print_vec3(glm::vec3 v);
  static void print_vec4(glm::vec4 v);
};

class FrameCounter {
public:
  FrameCounter(float interval_sec = 1.0f) { _interval_sec = interval_sec; }

  bool tick(double time_millis) {
    float time_secs = time_millis / 1000;
    _frame += 1;
    if (time_secs - _last_frame_secs > _interval_sec) {
      _frames_per_unit = _frame;
      _last_frame_secs = time_secs;
      _frame = 0;
      return true;
    }
    return false;
  }

  double fps() { return _frames_per_unit / _interval_sec; }

protected:
  int _frame = 0;
  float _last_frame_secs = 0;
  int _frames_per_unit;
  float _interval_sec;
};
