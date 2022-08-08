#include "engine/lua/usertypes/model/LTextureTypeInfo.h"
#include "engine/model/TextureTypeInfo.h"

namespace engine::lua::usertypes {
    void InitializeTextureTypeInfo(sol::state &L) {

        // Register the new usertype
        L.new_usertype<TextureTypeInfo>(
                // Name
                "TextureTypeInfo",

                sol::constructors<TextureTypeInfo(),
                TextureTypeInfo(std::string)>(),

                // Member variables
                "ShaderName", &TextureTypeInfo::ShaderName);
    }
}// namespace engine::lua::usertypes
