#pragma once

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <lua.hpp>
#include <LuaBridge/LuaBridge.h>
#include "engine/Base.h"

namespace engine::components {
    struct Transform {
        glm::vec3 Position = glm::vec3{0};
        glm::quat Rotation = glm::quat{0, 0, 0, 1};
        glm::vec3 Scale = glm::vec3{1};

        Transform() {}

        glm::mat4 GetTransformMatrix();

        luabridge::LuaRef GetTable(lua_State *L) const;
        void SetTable(luabridge::LuaRef Component);

        std::string GetName() const { return "Transform"; }
    };
}// namespace engine::components
