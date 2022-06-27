#include "engine/model/ModelLoader.h"
#include "engine/Base.h"
#include "engine/ecs/components/Model.h"
#include "engine/glcore/Shader.h"
#include "engine/model/Material.h"
#include "engine/ressources/RessourceManager.h"
#include <memory>
#include <stdexcept>

namespace engine {
    glcore::Shader ModelLoader::DefaultShader{};
    sptr<components::Model> ModelLoader::LoadModel(std::string Path) {
        LOG_ENGINE_TRACE("Loading model \"{0}\"", Path);

        Assimp::Importer Importer;
        const aiScene *Scene = Importer.ReadFile(Path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices);

        if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode) {
            LOG_ENGINE_ERROR("Failed to load model (\"{0}\")! Assimp Error: {1}", Path, Importer.GetErrorString());
            throw std::runtime_error("Failed to load model.");
        }

        auto BaseDir = Path.substr(0, Path.find_last_of("\\/"));

        auto CreatedModel = std::make_shared<components::Model>();
        ProcessNode(Scene->mRootNode, Scene, CreatedModel, BaseDir);

        return CreatedModel;
    }

    void ModelLoader::ProcessNode(aiNode *Node, const aiScene *Scene, sptr<components::Model> Model, std::string BaseDir) {
        for (unsigned int i = 0; i < Node->mNumMeshes; i++) {
            aiMesh *Mesh = Scene->mMeshes[Node->mMeshes[i]];
            Model->Meshes.push_back(ProcessMesh(Mesh, Scene, BaseDir));
        }

        for (unsigned int i = 0; i < Node->mNumChildren; i++) {
            ProcessNode(Node->mChildren[i], Scene, Model, BaseDir);
        }
    }

    Mesh ModelLoader::ProcessMesh(aiMesh *ProcessedMesh, const aiScene *Scene, std::string BaseDir) {
        std::vector<Vertex> Vertices;
        std::vector<unsigned int> Indicies;
        Material MeshMaterial;

        for (unsigned int i = 0; i < ProcessedMesh->mNumVertices; i++) {
            Vertex Vert;
            Vert.Position.x = ProcessedMesh->mVertices[i].x;
            Vert.Position.y = ProcessedMesh->mVertices[i].y;
            Vert.Position.z = ProcessedMesh->mVertices[i].z;

            Vert.Normal.x = ProcessedMesh->mNormals[i].x;
            Vert.Normal.y = ProcessedMesh->mNormals[i].y;
            Vert.Normal.z = ProcessedMesh->mNormals[i].z;

            if (ProcessedMesh->mTextureCoords[0]) {
                Vert.TexCoords.x = ProcessedMesh->mTextureCoords[0][i].x;
                Vert.TexCoords.y = ProcessedMesh->mTextureCoords[0][i].y;
            } else {
                Vert.TexCoords = glm::vec2{0, 0};
            }

            Vertices.push_back(Vert);
        }

        for (unsigned int i = 0; i < ProcessedMesh->mNumFaces; i++) {
            aiFace Face = ProcessedMesh->mFaces[i];
            for (unsigned int j = 0; j < Face.mNumIndices; j++) {
                Indicies.push_back(Face.mIndices[j]);
            }
        }

        if (ProcessedMesh->mMaterialIndex > 0) {
            aiMaterial *Mat = Scene->mMaterials[ProcessedMesh->mMaterialIndex];
            MeshMaterial = ProcessMaterial(Mat, BaseDir);
        }

        return Mesh{Vertices, Indicies, MeshMaterial};
    }

    Material ModelLoader::ProcessMaterial(aiMaterial *Mat, std::string BaseDir) {
        std::vector<std::pair<std::string, glcore::Texture2D>> Textures;
        auto diffuseMaps = LoadMaterialTexture(Mat, aiTextureType_DIFFUSE, "texture_diffuse", BaseDir);
        Textures.insert(Textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        auto specularMaps = LoadMaterialTexture(Mat, aiTextureType_SPECULAR, "texture_specular", BaseDir);
        Textures.insert(Textures.end(), specularMaps.begin(), specularMaps.end());

        return Material{DefaultShader, Textures};
    }

    std::vector<std::pair<std::string, glcore::Texture2D>> ModelLoader::LoadMaterialTexture(aiMaterial *Mat, aiTextureType Type, std::string TypeName, std::string BaseDir) {
        std::vector<std::pair<std::string, glcore::Texture2D>> Textures;

        for (unsigned int i = 0; i < Mat->GetTextureCount(Type); i++) {
            aiString str;
            Mat->GetTexture(Type, i, &str);

            std::string filename = str.C_Str();
            filename = BaseDir + '/' + filename;

            const auto &Data = RessourceManager::Get<glcore::TextureData>(filename);
            glcore::Texture2D Texture = glcore::Texture2D::FromTextureData(Data);
            Textures.push_back({TypeName, Texture});
        }
        return Textures;
    }

}// namespace engine
