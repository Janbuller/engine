#pragma once

#include "Vertex.h"
#include "engine/model/Material.h"
#include "glcore/Shader.h"
#include "glcore/Texture.h"
#include <vector>

namespace engine {
    class Mesh {
    public:
        std::vector<Vertex> Vertices;
        std::vector<unsigned int> Indicies;
        Material MeshMaterial;

        Mesh(std::vector<Vertex> Vertices, std::vector<unsigned int> Indicies, Material MeshMaterial);
        ~Mesh();

        void SetupBuffers();
        void DeleteBuffers();

        unsigned int VAO, VBO, EBO;
    };
}// namespace engine