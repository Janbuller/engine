#pragma once

#include "engine/glcore/Shader.h"
#include "engine/glcore/Texture2D.h"
#include "engine/model/TextureType.h"
#include "engine/model/TextureTypeInfo.h"
#include <array>
#include <string>
#include <utility>
#include <vector>

namespace engine {
    struct Material {
        glcore::Shader Shader;
        std::vector<std::pair<TextureType, glcore::Texture>> Textures;

        Material() = default;
        Material(glcore::Shader Shader, std::vector<std::pair<TextureType, glcore::Texture>> Textures) : Shader(Shader), Textures(Textures) {}

        static std::array<TextureTypeInfo, (int) TextureType::NONE> TextureInfo;
    };
}// namespace engine
