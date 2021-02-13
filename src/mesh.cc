#include "mesh.h"

#include <string>
#include <vector>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

Mesh::Mesh(std::vector<Vertex> verts,
       std::vector<unsigned int> indices,
       std::vector<Texture> textures) {
  this->vertices = verts;
  this->indices = indices;
  this->textures = textures;
  setupMesh();
}

void Mesh::draw(Shader& shader) {
  unsigned int diffuse_nr = 1;
  unsigned int specular_nr = 1;
  for (unsigned int i = 0; i < textures.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    std::string number;
    std::string name = textures[i].type;
    if (name == "texture_diffuse") {
      number = std::to_string(diffuse_nr++);
    } else if (name == "specular_diffuse") {
      number = std::to_string(specular_nr++);
    }

    //shader.setFloat(("material." + name + number).c_str(), i);
    shader.setInt((name + number).c_str(), i);
    glBindTexture(GL_TEXTURE_2D, textures[i].id);
  }
  glActiveTexture(GL_TEXTURE0);

  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  if (_debug) {
    glBindVertexArray(normals_vao);
    glDrawElements(GL_LINES, normals_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }
}

void Mesh::setupMesh() {
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);
  {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
                 &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int),
                 &indices[0], GL_STATIC_DRAW);
  }
  {
    // positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
        (void*)offsetof(Vertex, normal));
    // textures
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (void*)offsetof(Vertex, texture));
  }
  glBindVertexArray(0);
}

void Mesh::setupDrawNormals() {
  if (normals.size() == 0) {
    int i = 0;
    for (const Vertex& v: vertices) {
      Vertex a = v;
      Vertex b = v;
      b.pos = a.pos + a.normal/2.5f;
      normals.push_back(a);
      normals.push_back(b);
      normals_indices.push_back(i++);
      normals_indices.push_back(i++);
    }
  }

  glGenVertexArrays(1, &normals_vao);
  glGenBuffers(1, &normals_vbo);
  glGenBuffers(1, &normals_ebo);

  glBindVertexArray(normals_vao);
  {
    glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(Vertex),
                 &normals[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, normals_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
        normals_indices.size()*sizeof(unsigned int),
        &normals_indices[0], GL_STATIC_DRAW);
  }
  {
    // positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
        (void*)offsetof(Vertex, normal));
    // textures
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (void*)offsetof(Vertex, texture));
  }
  glBindVertexArray(0);
}
