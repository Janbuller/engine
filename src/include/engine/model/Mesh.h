#pragma once

#include "Vertex.h"
#include "engine/glcore/Shader.h"
#include "engine/glcore/Texture2D.h"
#include "engine/model/Material.h"
#include <vector>

namespace engine {
    class Mesh {
    public:
        std::vector<Vertex> Vertices;
        std::vector<unsigned int> Indices;
        Material MeshMaterial;

        Mesh(std::vector<Vertex> Vertices, std::vector<unsigned int> Indices, Material MeshMaterial);
        ~Mesh();

        void SetupBuffers();
        void DeleteBuffers();

        unsigned int VAO, VBO, EBO;
    };
}// namespace engine
