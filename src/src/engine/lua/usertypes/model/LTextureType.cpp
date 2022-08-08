#include "engine/lua/usertypes/model/LTextureType.h"
#include "engine/model/TextureType.h"

namespace engine::lua::usertypes {
    void InitializeTextureType(sol::state &L) {
        L.new_enum(
                // Name
                "TextureType",

                // Values
                "DIFFUSE", TextureType::DIFFUSE,
                "SPECULAR", TextureType::SPECULAR,
                "NORMAL", TextureType::NORMAL);

    }
}// namespace engine::lua::usertypes
