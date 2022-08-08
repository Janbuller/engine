#pragma once

#include <glm/glm.hpp>

namespace engine {
    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
        glm::vec3 Tangent;

        Vertex() = default;
        Vertex(glm::vec3 Position,
               glm::vec3 Normal,
               glm::vec2 TexCoords,
               glm::vec3 Tangent) : Position{Position},
                                    Normal{Normal},
                                    TexCoords{TexCoords},
                                    Tangent{Tangent} {}
    };
}// namespace engine
