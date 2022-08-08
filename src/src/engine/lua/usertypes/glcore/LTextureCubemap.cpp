#include "engine/lua/usertypes/glcore/LTextureCubemap.h"
#include "engine/glcore/TextureCubemap.h"

namespace engine::lua::usertypes {
    void InitializeTextureCubemap(sol::state &L) {

        // Register the new usertype
        L.new_usertype<glcore::TextureCubemap>(
                // Name
                "TextureCubemap",

                sol::constructors<glcore::TextureCubemap(),
                                  glcore::TextureCubemap(GLuint)>(),

                // Member variables
                "Handle", &glcore::Texture::Handle,

                "FromTextureData", &glcore::TextureCubemap::FromTextureData,

                sol::base_classes, sol::bases<glcore::Texture>());
    }
}// namespace engine::lua::usertypes
