#pragma once

#include <glad/glad.h>

namespace engine::glcore {
    struct Texture {
        GLuint Handle;

        int Width, Height;

        Texture() = default;
        Texture(GLuint Handle, int Width, int Height);
    };
}
