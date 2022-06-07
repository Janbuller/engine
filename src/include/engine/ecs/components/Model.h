#pragma once

#include "engine/Mesh.h"
#include "engine/Vertex.h"
#include <lua.hpp>
#include <LuaBridge/LuaBridge.h>
#include <vector>
#include "engine/Base.h"

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
