#include "engine/Mesh.h"
#include "engine/Material.h"
#include "engine/Vertex.h"
#include "glcore/Shader.h"
#include "glcore/Texture.h"
#include <fstream>
#include <glad/glad.h>
#include <sstream>
#include <vector>

namespace engine {
    Mesh::Mesh(std::vector<Vertex> Vertices, std::vector<unsigned int> Indicies, Material MeshMaterial) : Vertices(Vertices), Indicies(Indicies), MeshMaterial(MeshMaterial) {
        SetupBuffers();
    }

    void Mesh::SetupBuffers() {
        glCreateVertexArrays(1, &VAO);
        glCreateBuffers(1, &VBO);
        glCreateBuffers(1, &EBO);

        glNamedBufferData(VBO, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);
        glNamedBufferData(EBO, Indicies.size() * sizeof(unsigned int), &Indicies[0], GL_STATIC_DRAW);

        glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(Vertex));
        glVertexArrayElementBuffer(VAO, EBO);

        glEnableVertexArrayAttrib(VAO, 0);
        glEnableVertexArrayAttrib(VAO, 1);
        glEnableVertexArrayAttrib(VAO, 2);

        glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Position));
        glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Normal));
        glVertexArrayAttribFormat(VAO, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, TexCoords));

        glVertexArrayAttribBinding(VAO, 0, 0);
        glVertexArrayAttribBinding(VAO, 1, 0);
        glVertexArrayAttribBinding(VAO, 2, 0);
    }

    void Mesh::Draw(glcore::Shader &shader) {
        int diffuseNr = 1;

        for (unsigned int i = 0; i < MeshMaterial.Textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);

            std::string number;
            std::string type = "texture_diffuse";
            if (type == "texture_diffuse") {
                number = std::to_string(diffuseNr++);

                shader.SetInt(("material." + type + number).c_str(), i);
                glBindTexture(GL_TEXTURE_2D, MeshMaterial.Textures[i].second.id);
            }
            glActiveTexture(GL_TEXTURE0);

            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, Indicies.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    }

}// namespace engine
