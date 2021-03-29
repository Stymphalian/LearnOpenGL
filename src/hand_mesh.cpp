#include "hand_mesh.h"

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include "mesh.h"

Mesh create_square_mesh(std::vector<unsigned int> texs) {
  vector<Vertex> verts = {
      {{-0.5f, 0.5f, 0.0f}, {0,0,0}, {-1,1}},
      {{0.5f, 0.5f, 0.0f},  {0,0,0}, {1, 1}},
      {{0.5f, -0.5f, 0.0f}, {0,0,0}, {1,-1}},
      {{-0.5f, -0.5f, 0.0f},{0,0,0}, {-1,-1}},
  };
  vector<unsigned int> indices = {
    0, 1, 2,
    0, 2, 3,
  };
  vector<Texture> textures;
  for (unsigned int t: texs) {
    textures.push_back({t, "texture_diffuse"});
  }
  return Mesh(verts, indices, textures);
}


vector<glm::vec3> calculate_vertex_normals(
    const vector<Vertex>& m, 
    const vector<unsigned int>& tri_idx) {
  vector<glm::vec3> normals(m.size());

  for (int i = 0; i < tri_idx.size(); i += 3) {
    unsigned int ai = tri_idx[i];
    unsigned int bi = tri_idx[i+1];
    unsigned int ci = tri_idx[i+2];

    const glm::vec3& a = m.at(ai).pos;
    const glm::vec3& b = m.at(bi).pos;
    const glm::vec3& c = m.at(ci).pos;

    normals.at(ai) += glm::cross(b-a, c-a);
    normals.at(bi) += glm::cross(c-b, a-b);
    normals.at(ci) += glm::cross(a-c, b-c);
    //normals.at(ai) += glm::cross(c-a, b-a);
    //normals.at(bi) += glm::cross(a-b, c-b);
    //normals.at(ci) += glm::cross(b-c, a-c);
  }

  for(glm::vec3& v: normals) {
    v = glm::normalize(v);
  }
  return normals;
}

Mesh create_cube_mesh(std::vector<unsigned int> texs) {
  vector<Texture> textures;
  for (unsigned int t: texs) {
    textures.push_back(Texture(t, "texture_diffuse"));
  }

  vector<Vertex> cube = {
      // front face
      {{-0.5f, 0.5f, 0.5f}, {0,0,0}, {-1,1}},
      {{0.5f, 0.5f, 0.5f},  {0,0,0}, {1, 1}},
      {{0.5f, -0.5f, 0.5f}, {0,0,0}, {1,-1}},
      {{-0.5f, -0.5f, 0.5f},{0,0,0}, {-1,-1}},
      // back face
      {{-0.5f, 0.5f, -0.5f}, {0,0,0}, {-1,1}},
      {{0.5f, 0.5f, -0.5f},  {0,0,0}, {1, 1}},
      {{0.5f, -0.5f, -0.5f}, {0,0,0}, {1,-1}},
      {{-0.5f, -0.5f, -0.5f},{0,0,0}, {-1,-1}},
  };
  vector<unsigned int> idx = {
    // front
    0,1,2,
    0,2,3,
    // left
    4,0,3,
    4,3,7,
    // botttom
    3,2,6,
    3,6,7,
    // back
    5,4,7,
    5,7,6,
    // right
    1,5,6,
    1,6,2,
    // top
    4,5,1,
    4,1,0
  };

  vector<glm::vec3> vertex_normals = calculate_vertex_normals(cube, idx);
  for (int i = 0; i < cube.size(); i++) {
    cube[i].normal = vertex_normals[i];
  }
  return Mesh(cube, idx, textures);
}


Mesh create_cube_mesh2(std::map<string, unsigned int> texs) {
  vector<Texture> textures;
  for (const auto& e: texs) {
    textures.push_back({e.second, e.first});
    //textures.push_back({t, "texture_diffuse"});
    //textures.push_back({t, "material.diffuse"});
  }

  vector<Vertex> cube = {
      // front face
      {{-0.5f, 0.5f, 0.5f}, {0,0,0}, {0,1}},
      {{0.5f, 0.5f, 0.5f},  {0,0,0}, {1, 1}},
      {{0.5f, -0.5f, 0.5f}, {0,0,0}, {1,0}},
      {{-0.5f, -0.5f, 0.5f},{0,0,0}, {0,0}},
      // back face
      {{0.5f, 0.5f, -0.5f}, {0,0,0}, {0,1}},
      {{-0.5f, 0.5f, -0.5f},  {0,0,0}, {1, 1}},
      {{-0.5f, -0.5f, -0.5f}, {0,0,0}, {1,0}},
      {{0.5f, -0.5f, -0.5f},{0,0,0}, {0,0}},
      // left face
      {{-0.5f, 0.5f, -0.5f}, {0,0,0}, {0,1}},
      {{-0.5f, 0.5f, 0.5f},  {0,0,0}, {1, 1}},
      {{-0.5f, -0.5f, 0.5f}, {0,0,0}, {1,0}},
      {{-0.5f, -0.5f, -0.5f},{0,0,0}, {0,0}},
      // right face
      {{0.5f, 0.5f, 0.5f}, {0,0,0}, {0,1}},
      {{0.5f, 0.5f, -0.5f},  {0,0,0}, {1, 1}},
      {{0.5f, -0.5f, -0.5f}, {0,0,0}, {1,0}},
      {{0.5f, -0.5f, 0.5f},{0,0,0}, {0,0}},
      // top face
      {{-0.5f, 0.5f, -0.5f}, {0,0,0}, {0,1}},
      {{0.5f, 0.5f, -0.5f},  {0,0,0}, {1, 1}},
      {{0.5f, 0.5f, 0.5f}, {0,0,0}, {1,0}},
      {{-0.5f,0.5f, 0.5f},{0,0,0}, {0,0}},
      // bot face
      {{-0.5f, -0.5f, 0.5f}, {0,0,0}, {0,1}},
      {{0.5f, -0.5f, 0.5f},  {0,0,0}, {1, 1}},
      {{0.5f, -0.5f, -0.5f}, {0,0,0}, {1,0}},
      {{-0.5f,-0.5f, -0.5f},{0,0,0}, {0,0}},
  };
  //vector<unsigned int> idx = {
  //  // front
  //  0,1,2,
  //  0,2,3,
  //  // back
  //  4,5,6,
  //  4,6,7,
  //  // left
  //  8,9,10,
  //  8,10,11,
  //  // right
  //  12,13,14,
  //  12,14,15,
  //  // top
  //  16,17,18,
  //  16,18,19,
  //  // bot
  //  20,21,22,
  //  20,22,23
  //};
  vector<unsigned int> idx = {
    // front
    0,2,1,
    0,3,2,
    // back
    4,6,5,
    4,7,6,
    // left
    8,10,9,
    8,11,10,
    // right
    12,14,13,
    12,15,14,
    // top
    16,18,17,
    16,19,18,
    // bot
    20,22,21,
    20,23,22
  };

  // Assign the normals into the Vertex object
  vector<glm::vec3> vertex_normals = calculate_vertex_normals(cube, idx);
  for (int i = 0; i < cube.size(); i++) {
    cube[i].normal = vertex_normals[i];
  }

  return Mesh(cube, idx, textures);
}

