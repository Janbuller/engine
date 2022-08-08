#include "engine/lua/usertypes/glcore/LTexture2D.h"
#include "engine/glcore/Texture2D.h"

namespace engine::lua::usertypes {
    void InitializeTexture2D(sol::state &L) {

        // Register the new usertype
        L.new_usertype<glcore::Texture2D>(
                // Name
                "Texture2D",

                sol::constructors<glcore::Texture2D(),
                                  glcore::Texture2D(GLuint, int, int)>(),

                // Member variables
                "Handle", &glcore::Texture::Handle,

                "Width",  &glcore::Texture2D::Width,
                "Height", &glcore::Texture2D::Height,

                "FromTextureData", &glcore::Texture2D::FromTextureData,

                sol::base_classes, sol::bases<glcore::Texture>());
    }
}// namespace engine::lua::usertypes
