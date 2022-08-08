#include "engine/lua/usertypes/model/LMaterial.h"
#include "engine/model/Material.h"

namespace engine::lua::usertypes {
    void InitializeMaterial(sol::state &L) {

        // Register the new usertype
        L.new_usertype<Material>(
                // Name
                "Material",

                sol::constructors<Material(),
                                  Material(glcore::Shader, std::vector<TypedTexture>),
                                  Material(const Material&)>(),

                // Member variables
                "Shader",      &Material::Shader,
                "Textures",    &Material::Textures,
                "TextureInfo", sol::var(&Material::TextureInfo));
    }
}// namespace engine::lua::usertypes
