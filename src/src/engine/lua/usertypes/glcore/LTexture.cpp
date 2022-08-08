#include "engine/lua/usertypes/glcore/LTexture.h"
#include "engine/glcore/Texture.h"

namespace engine::lua::usertypes {
    void InitializeTexture(sol::state &L) {

        // Register the new usertype
        L.new_usertype<glcore::Texture>(
                // Name
                "Texture",

                sol::constructors<glcore::Texture(),
                                  glcore::Texture(GLuint)>(),

                // Member variables
                "Handle", &glcore::Texture::Handle);
    }
}// namespace engine::lua::usertypes
