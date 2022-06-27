#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace engine {
    struct Light {
        alignas(16) glm::vec4 position;

        alignas(16) glm::vec3 color;
        GLfloat intensity;

        GLfloat constant;
        GLfloat linear;
        GLfloat quadratic;
    };
}// namespace engine
