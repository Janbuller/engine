#include "engine/model/Mesh.h"
#include "engine/model/Material.h"
#include "engine/model/Vertex.h"
#include "engine/Base.h"
#include "engine/glcore/Shader.h"
#include "engine/glcore/Texture2D.h"
#include <fstream>
#include <glad/glad.h>
#include <sstream>
#include <vector>

namespace engine {
    Mesh::Mesh(std::vector<Vertex> Vertices, std::vector<unsigned int> Indices, Material MeshMaterial) : Vertices(Vertices), Indices(Indices), MeshMaterial(MeshMaterial) {
        SetupBuffers();
    }

  Mesh::~Mesh() {
    DeleteBuffers();
  }

    void Mesh::SetupBuffers() {
        glCreateVertexArrays(1, &VAO);
        glCreateBuffers(1, &VBO);
        glCreateBuffers(1, &EBO);

        glNamedBufferData(VBO, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);
        glNamedBufferData(EBO, Indices.size() * sizeof(unsigned int), &Indices[0], GL_STATIC_DRAW);

        glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(Vertex));
        glVertexArrayElementBuffer(VAO, EBO);

        glEnableVertexArrayAttrib(VAO, 0);
        glEnableVertexArrayAttrib(VAO, 1);
        glEnableVertexArrayAttrib(VAO, 2);
        glEnableVertexArrayAttrib(VAO, 3);

        glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Position));
        glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Normal));
        glVertexArrayAttribFormat(VAO, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, TexCoords));
        glVertexArrayAttribFormat(VAO, 3, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Tangent));

        glVertexArrayAttribBinding(VAO, 0, 0);
        glVertexArrayAttribBinding(VAO, 1, 0);
        glVertexArrayAttribBinding(VAO, 2, 0);
        glVertexArrayAttribBinding(VAO, 3, 0);
    }

  void Mesh::DeleteBuffers() {
    // glDeleteBuffers(1, &EBO);
    // glDeleteBuffers(1, &VBO);
    // glDeleteVertexArrays(1, &VAO);
  }
}// namespace engine
