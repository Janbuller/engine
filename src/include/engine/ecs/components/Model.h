#pragma once

#include "engine/model/Mesh.h"
#include "engine/model/Vertex.h"
#include "engine/Base.h"

// clang-format off
#include <lua.hpp>
#include <LuaBridge/LuaBridge.h>
// clang-format on

#include <vector>

namespace engine::components {
    struct Model {
        bool Visible = true;
        std::vector<Mesh> Meshes;

        Model() {
            Visible = true;
        }

        luabridge::LuaRef GetTable(lua_State *L) const;
        void SetTable(luabridge::LuaRef Component);

        std::string GetName() const { return "Model"; }

        static Model Load(std::string Path);
    };
}// namespace engine::components
