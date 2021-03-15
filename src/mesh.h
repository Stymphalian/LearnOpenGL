#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>
#include <iostream>
#include <string>

#include <glm/glm.hpp>

#include "misc.h"
#include "shader.h"

struct Vertex {
  glm::vec3 pos;
  glm::vec3 normal;
  glm::vec2 texture;

  std::string to_string() {
    return string_format(
        "{(% 04.2f,% 04.2f,% 04.2f),(% 04.2f,% 04.2f,% 04.2f),(% 04.2f,% 04.2f)}",
        pos.x, pos.y, pos.z, 
        normal.x, normal.y, normal.z, 
        texture.x, texture.y);
  }
};

struct Texture {
  unsigned int id;
  std::string type;
  std::string textureName;

  Texture() {}
  Texture(unsigned int id, std::string type): id(id),type(type){}
};

class Mesh {
public:
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;
  bool _debug = false;

  Mesh(std::vector<Vertex> verts,
       std::vector<unsigned int> indices,
       std::vector<Texture> textures);

  void draw(Shader& shader);
  void debug(bool on) { 
    _debug = on;
    if (_debug) {
      setupDrawNormals();
    }
  } 

private:
  unsigned int vao;
  unsigned int vbo;
  unsigned int ebo;

  unsigned int normals_vao;
  unsigned int normals_vbo;
  unsigned int normals_ebo;
  std::vector<Vertex> normals;
  std::vector<unsigned int> normals_indices;

  void setupMesh();
  void setupDrawNormals();
};

#endif
