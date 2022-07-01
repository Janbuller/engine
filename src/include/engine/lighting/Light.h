#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace engine {
    struct Light {
        alignas(16) glm::vec4 Position;

        alignas(16) glm::vec3 Color;
        GLfloat Intensity;

        GLfloat Constant;
        GLfloat Linear;
        GLfloat Quadratic;
    };
}// namespace engine
