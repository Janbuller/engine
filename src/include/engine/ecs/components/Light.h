#pragma once

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glad/glad.h>

namespace engine::components {
    struct Light {
        alignas(16) glm::vec4 Position;

        alignas(16) glm::vec3 Color;
        GLfloat Intensity;

        GLfloat Constant;
        GLfloat Linear;
        GLfloat Quadratic;
    };
}// namespace engine::components
