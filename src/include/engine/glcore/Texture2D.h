#pragma once

#include "engine/glcore/Texture.h"
#include "engine/glcore/TextureData.h"
#include <glad/glad.h>
#include <string>

namespace engine::glcore {
    struct Texture2D : public Texture {
        int Width;
        int Height;

        Texture2D() = default;
        Texture2D(GLuint Handle, int Width, int Height);
        static Texture2D FromTextureData(TextureData TD);
    };
}// namespace engine::glcore
