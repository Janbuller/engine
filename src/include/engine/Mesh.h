#pragma once

#include "Vertex.h"
#include "engine/Material.h"
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
        void Draw(glcore::Shader &shader);

        void SetupBuffers();
    private:
        unsigned int VAO, VBO, EBO;

    };
}// namespace engine
