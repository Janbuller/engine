#pragma once

#include "engine/glcore/Texture.h"
#include "engine/model/TextureType.h"

namespace engine {
    struct TypedTexture {
        TextureType Type;
        glcore::Texture Texture;
    };
}
