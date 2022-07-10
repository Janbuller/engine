#pragma once

#include "engine/Base.h"

// clang-format off
#include <lua.hpp>
#include <LuaBridge/LuaBridge.h>
// clang-format on

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace engine::components {
    struct Transform {
        glm::vec3 Position = glm::vec3{0};
        glm::quat Rotation = glm::quat{1, 0, 0, 0};
        glm::vec3 Scale = glm::vec3{1};

        Transform() = default;

        glm::mat4 GetTransformMatrix() const;

        luabridge::LuaRef GetTable(lua_State *L) const;
        void SetTable(luabridge::LuaRef Component);

        std::string GetName() const { return "Transform"; }
    };
}// namespace engine::components
