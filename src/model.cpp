#include "model.h"

#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "shader.h"

using namespace std;

void Model::draw(Shader &shader) {
  for (int i = 0; i < meshes.size(); i++) {
    meshes[i].draw(shader);
  }
}

void Model::loadModel(std::string path) {
  Assimp::Importer import;
  const aiScene *scene =
      import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
    return;
  }
  directory = path.substr(0, path.find_last_of('/'));

  processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
  // process all the node's meshes (if any)
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    meshes.push_back(processMesh(mesh, scene));
  }
  // then do the same for each of its children
  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    processNode(node->mChildren[i], scene);
  }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
  vector<Vertex> vertices(mesh->mNumVertices);
  vector<unsigned int> indices;
  vector<Texture> textures;

  // Process all the vertices
  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;

    vertex.pos.x = mesh->mVertices[i].x;
    vertex.pos.y = mesh->mVertices[i].y;
    vertex.pos.z = mesh->mVertices[i].z;

    vertex.normal.x = mesh->mNormals[i].x;
    vertex.normal.y = mesh->mNormals[i].y;
    vertex.normal.z = mesh->mNormals[i].z;

    if (mesh->mTextureCoords[0]) {
      vertex.texture.x = mesh->mTextureCoords[0][i].x;
      vertex.texture.y = mesh->mTextureCoords[0][i].y;
    } else {
      vertex.texture = glm::vec2(0.0f, 0.0f);
    }

    vertices[i] = vertex;
  }

  // Process the meshes
  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++) {
      indices.push_back(face.mIndices[j]);
    }
  }

  // Process the materials
  if (mesh->mMaterialIndex >= 0) {
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

    vector<Texture> diffuseMaps = loadMaterialTextures(
        material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    vector<Texture> specularMaps = loadMaterialTextures(
        material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

    vector<Texture> emissionMaps = loadMaterialTextures(
        material, aiTextureType_EMISSIVE, "texture_emission");
    textures.insert(textures.end(), emissionMaps.begin(), emissionMaps.end());
  }

  return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat,
                                                 aiTextureType type,
                                                 std::string typeName) {
  vector<Texture> textures;
  for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
    aiString textureFilename;
    mat->GetTexture(type, i, &textureFilename);
    string textureName = textureFilename.C_Str();

    if (textures_loaded.count(textureName) > 0) {
      textures.push_back(textures_loaded.at(textureName));
    } else {
      std::experimental::filesystem::path p(directory);
      p /= textureName;

      Texture texture;
      texture.id = load_texture(p.c_str());
      texture.type = typeName;
      texture.textureName = textureName;
      textures.push_back(texture);

      // optimization, save the texture by its filename so we can re-use
      // it in the model
      textures_loaded.insert(
          std::pair<string, Texture>(texture.textureName, texture));
    }
  }
  return textures;
}
