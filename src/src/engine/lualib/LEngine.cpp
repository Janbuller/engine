#include "engine/lualib/LEngine.h"
#include "engine/lualib/LMath.h"
#include <sol/sol.hpp>

namespace engine::lualib {
    void LoadEngineLib(sol::state &L) {
        sol::table Engine = L["Engine"].get_or_create<sol::table>();
        LoadMathLib(Engine);
    }
}// namespace engine::lualib
