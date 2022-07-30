#pragma once

#include <glad/glad.h>

namespace engine::glcore {
    struct Texture {
        GLuint Handle;

        Texture() = default;
        Texture(GLuint Handle);
    };
}// namespace engine::glcore
