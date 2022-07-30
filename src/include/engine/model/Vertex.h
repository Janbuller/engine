#pragma once

#include <glm/glm.hpp>

namespace engine {
    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
        glm::vec3 Tangent;
    };
}// namespace engine
