#pragma once
#include <string>
#include <map>
#include <vector>

#include <assimp/scene.h>

#include "mesh.h"
#include "shader.h"


class Model {
public:
  Model(std::string path) { loadModel(path); }
  Model(Mesh mesh) { meshes.push_back(mesh); }

  void draw(Shader &shader);

private:
  std::vector<Mesh> meshes;
  std::string directory;
  std::map<std::string, Texture> textures_loaded;

  void loadModel(std::string path);
  void processNode(aiNode *node, const aiScene *scene);
  Mesh processMesh(aiMesh *mesh, const aiScene *scene);
  std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                            std::string typeName);
};
