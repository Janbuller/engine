#pragma once

#include "Vertex.h"
#include "engine/model/Material.h"
#include <vector>

namespace engine {
    class Mesh {
    public:
        std::vector<Vertex> Vertices;
        std::vector<unsigned int> Indices;
        Material MeshMaterial;

        Mesh() = default;
        Mesh(std::vector<Vertex> Vertices, std::vector<unsigned int> Indices, Material MeshMaterial);
        ~Mesh();

        void SetupBuffers();
        void DeleteBuffers();

        unsigned int VAO, VBO, EBO;
    };
}// namespace engine
