#include <sol/sol.hpp>

namespace engine::lua::lualib {
    void LoadLogLib(sol::table &L);

    void Trace(sol::this_state State, sol::variadic_args ToPrint);
    void Info(sol::this_state State, sol::variadic_args ToPrint);
    void Warn(sol::this_state State, sol::variadic_args ToPrint);
    void Error(sol::this_state State, sol::variadic_args ToPrint);
    void Critical(sol::this_state State, sol::variadic_args ToPrint);
    std::string ArgsToString(sol::state_view L, sol::variadic_args ToPrint);
}// namespace engine::lualib
