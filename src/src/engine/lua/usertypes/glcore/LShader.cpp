#include "engine/lua/usertypes/glcore/LShader.h"
#include "engine/glcore/Shader.h"

namespace engine::lua::usertypes {
    void InitializeShader(sol::state &L) {

        // Register the new usertype
        L.new_usertype<glcore::Shader>(
                // Name
                "Shader",

                sol::constructors<glcore::Shader(),
                glcore::Shader(const char*, const char*)>(),

                "Bind", &glcore::Shader::Bind,
                
                "SetBool", &glcore::Shader::SetBool,
                "SetInt", &glcore::Shader::SetInt,
                "SetFloat", &glcore::Shader::SetFloat,
                "SetVec2", sol::overload(
                    sol::resolve<void(const std::string&, float, float) const>(&glcore::Shader::SetVec2),
                    sol::resolve<void(const std::string&, const glm::vec2&) const>(&glcore::Shader::SetVec2)
                    ),
                "SetVec3", sol::overload(
                    sol::resolve<void(const std::string&, float, float, float) const>(&glcore::Shader::SetVec3),
                    sol::resolve<void(const std::string&, const glm::vec3&) const>(&glcore::Shader::SetVec3)
                    ),
                /* "SetVec3", &glcore::Shader::SetVec3, */
                "SetMat4", &glcore::Shader::SetMat4,

                "Reload", &glcore::Shader::Reload

                );
    }
}// namespace engine::lua::usertypes
