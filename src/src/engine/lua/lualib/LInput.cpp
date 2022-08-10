#include "engine/lua/lualib/LInput.h"
#include "engine/glcore/Window.h"
#include "glm/glm.hpp"
#include <sol/sol.hpp>

namespace engine::lua::lualib {
    void LoadInputLib(sol::table &L, glcore::Window *InputWindow) {
        auto Log = L["Input"].get_or_create<sol::table>();

        Log["GetMousePos"] = [InputWindow]() -> glm::vec2 {
            auto Mouse = InputWindow->GetCursorPos();
            return glm::vec2{Mouse[0], Mouse[1]};
        };

        Log["IsKeyDown"] = [InputWindow](int KeyNum) -> bool {
            return InputWindow->GetKeyState(KeyNum) == engine::glcore::Window::KeyState::KEY_PRESS;
        };
    }
}// namespace engine::lua::lualib
