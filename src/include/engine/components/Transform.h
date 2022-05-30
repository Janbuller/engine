#pragma once

#include "engine/components/ILuaComponent.h"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "LuaBridge/detail/LuaRef.h"
#include <LuaBridge/LuaBridge.h>
#include <lua.hpp>

namespace engine::components {
    struct Transform {
        glm::vec3 Position = glm::vec3{0};
        glm::quat Rotation = glm::quat{0, 0, 0, 1};
        glm::vec3 Scale = glm::vec3{1};

        Transform() {}

        luabridge::LuaRef GetTable(lua_State *L) const {
            auto L_Transform = luabridge::newTable(L);
            L_Transform["Position"] = luabridge::newTable(L);
            L_Transform["Position"]["x"] = Position.x;
            L_Transform["Position"]["y"] = Position.y;
            L_Transform["Position"]["z"] = Position.z;

            L_Transform["Rotation"] = luabridge::newTable(L);
            L_Transform["Rotation"]["x"] = Rotation.x;
            L_Transform["Rotation"]["y"] = Rotation.y;
            L_Transform["Rotation"]["z"] = Rotation.z;
            L_Transform["Rotation"]["w"] = Rotation.w;

            L_Transform["Scale"] = luabridge::newTable(L);
            L_Transform["Scale"]["x"] = Scale.x;
            L_Transform["Scale"]["y"] = Scale.y;
            L_Transform["Scale"]["z"] = Scale.z;

            return L_Transform;
        };

        void SetTable(luabridge::LuaRef Component) {
            Position.x = Component["Position"]["x"];
            Position.y = Component["Position"]["y"];
            Position.z = Component["Position"]["z"];

            Rotation.x = Component["Rotation"]["x"];
            Rotation.y = Component["Rotation"]["y"];
            Rotation.z = Component["Rotation"]["z"];
            Rotation.w = Component["Rotation"]["w"];

            Scale.x = Component["Scale"]["x"];
            Scale.y = Component["Scale"]["y"];
            Scale.z = Component["Scale"]["z"];
        };

        std::string GetName() const {
            return "Transform";
        }
    };
}// namespace engine::components
