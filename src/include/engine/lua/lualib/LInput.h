#include "engine/glcore/Window.h"
#include <sol/sol.hpp>

namespace engine::lua::lualib {
    void LoadInputLib(sol::table &L, glcore::Window *InputWindow);
}// namespace engine::lualib
