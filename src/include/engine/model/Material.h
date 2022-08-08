#pragma once

#include "engine/glcore/Shader.h"
#include "engine/glcore/Texture.h"
#include "engine/glcore/Texture2D.h"
#include "engine/model/TextureType.h"
#include "engine/model/TextureTypeInfo.h"
#include "engine/model/TypedTexture.h"
#include <array>
#include <string>
#include <utility>
#include <vector>

namespace engine {
    struct Material {
        glcore::Shader Shader;
        std::vector<TypedTexture> Textures;

        Material() = default;
        Material(glcore::Shader Shader, std::vector<TypedTexture> Textures) : Shader{Shader}, Textures{Textures} {}
        Material(const Material& OldMat) : Shader{OldMat.Shader}, Textures{OldMat.Textures} {};

        static std::array<TextureTypeInfo, (int) TextureType::NONE> TextureInfo;
    };
}// namespace engine
