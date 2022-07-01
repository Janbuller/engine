
#pragma once

#include "engine/glcore/Texture.h"
#include "engine/glcore/TextureData.h"
#include <array>
#include <glad/glad.h>
#include <string>

namespace engine::glcore {
    struct TextureCubemap : public Texture {
        TextureCubemap () = default;
        TextureCubemap (GLuint Handle);
        static TextureCubemap FromTextureData(std::array<TextureData, 6> TD);
    };
}// namespace engine::glcore
