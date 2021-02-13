#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "mesh.h"
using namespace std;

Mesh create_square_mesh(std::vector<unsigned int> texs); 
vector<glm::vec3> calculate_vertex_normals(
    const vector<Vertex>& m, 
    const vector<unsigned int>& tri_idx); 
Mesh create_cube_mesh(std::vector<unsigned int> texs); 
Mesh create_cube_mesh2(std::vector<unsigned int> texs); 

