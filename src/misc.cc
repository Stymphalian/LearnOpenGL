#include "misc.h"

#include <experimental/filesystem>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "stb_image.h"

unsigned int load_texture(std::string image_filepath) {
  std::experimental::filesystem::path p(image_filepath);

  unsigned int texture_id;
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  int width, height, nr_channels;
  stbi_set_flip_vertically_on_load(1);
  unsigned char *data =
      stbi_load(image_filepath.c_str(), &width, &height, &nr_channels, 0);
  if (data) {
    if (p.extension() == ".png") {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA,
                   GL_UNSIGNED_BYTE, data);
    } else {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                   GL_UNSIGNED_BYTE, data);
    }

    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    return texture_id;
  } else {
    printf("Failed to load texture %s.\n", "container.jpg");
    return -1;
  }
}

void Util::print_mat4(glm::mat4 m) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      std::cout << m[i][j] << " ";
    }
    std::cout << std::endl;
  }
}
void Util::print_quat(glm::quat q) {
  printf("(%f, %f, %f, %f)\n", q.w, q.x, q.y, q.z);
}
void Util::print_vec3(glm::vec3 v) { printf("%f %f %f\n", v.x, v.y, v.z); }
void Util::print_vec4(glm::vec4 v) {
  printf("%f %f %f %f\n", v.w, v.x, v.y, v.z);
}
