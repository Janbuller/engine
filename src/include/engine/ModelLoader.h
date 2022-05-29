#pragma once

#include "engine/Mesh.h"
#include "engine/components/Model.h"
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace engine {
  class ModelLoader {
  public:
    components::Model LoadModel(std::string path);

  private:
    void ProcessNode(aiNode *node, const aiScene *scene);
    Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<glcore::Texture> LoadMaterialTexture(aiMaterial *mat, aiTextureType type, std::string typeName);
  };
}
