#include "engine/lua/lualib/LEngine.h"
#include "engine/lua/lualib/LLog.h"
#include "engine/lua/lualib/LMath.h"

#include "engine/core/Logger.h"
#include <sol/sol.hpp>

namespace engine::lua::lualib {
    void LoadEngineLib(sol::state &L) {
        sol::table Engine = L["Engine"].get_or_create<sol::table>();
        LoadMathLib(Engine);
        LoadLogLib(Engine);

        LOG_ENGINE_TRACE("Loaded Engine Lua Library");
    }
}// namespace engine::lua::lualib
