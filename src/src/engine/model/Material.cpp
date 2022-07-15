#include "engine/model/Material.h"

namespace engine {
    std::array<TextureTypeInfo, (int) TextureType::NONE> Material::TextureInfo{{
            {"T_Diffuse"},
            {"T_Specular"},
            {"T_Normal"},
    }};
}
