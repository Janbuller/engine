#pragma once

#include "engine/Mesh.h"
#include <lua.hpp>
#include <LuaBridge/LuaBridge.h>
#include <vector>

namespace engine::components {
    struct Model {
        bool Visible = true;
        std::vector<Mesh> Meshes;

        Model() {
            Visible = true;
        }

        luabridge::LuaRef GetTable(lua_State *L) const {
            auto L_Transform = luabridge::newTable(L);
            L_Transform["Visible"] = Visible;
            return L_Transform;
        };

        void SetTable(luabridge::LuaRef Component) {
            Visible = Component["Visible"];
        };

        std::string GetName() const {
            return "Model";
        }

        static Model Load(std::string Path);
    };
}// namespace engine::components
