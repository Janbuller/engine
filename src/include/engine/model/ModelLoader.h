#pragma once

#include "engine/Base.h"
#include "engine/model/Material.h"
#include "engine/model/Mesh.h"
#include "engine/ecs/components/Model.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "engine/ecs/components/Model.h"
#include <string>
#include <vector>

namespace engine {
    class ModelLoader {
    public:
        static sptr<components::Model> LoadModel(std::string Path);

        static glcore::Shader DefaultShader;

    private:
      static void ProcessNode(aiNode *Node, const aiScene *Scene, sptr<components::Model> Model, std::string BaseDir);
      static Mesh ProcessMesh(aiMesh *Mesh, const aiScene *Scene, std::string BaseDir);
      static Material ProcessMaterial(aiMaterial *Mat, std::string BaseDir);
      static std::vector<std::pair<std::string, glcore::Texture2D>> LoadMaterialTexture(aiMaterial *Mat, aiTextureType Type, std::string TypeName, std::string BaseDir);
    };
}// namespace engine
