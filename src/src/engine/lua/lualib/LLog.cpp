#include "engine/lua/lualib/LLog.h"
#include "engine/Base.h"
#include <sol/sol.hpp>

namespace engine::lua::lualib {
    void LoadLogLib(sol::table &L) {
        auto Log = L["Log"].get_or_create<sol::table>();

        Log["Trace"]    = Trace;
        Log["Info"]     = Info;
        Log["Warn"]     = Warn;
        Log["Error"]    = Error;
        Log["Critical"] = Critical;
    }

    void Trace(sol::this_state State, sol::variadic_args ToPrint) {
        sol::state_view StateView = State;

        auto FinalOutput = ArgsToString(StateView, ToPrint);

        LOG_TRACE("{0}", FinalOutput);
    }
    void Info(sol::this_state State, sol::variadic_args ToPrint) {
        sol::state_view StateView = State;

        auto FinalOutput = ArgsToString(StateView, ToPrint);

        LOG_INFO("{0}", FinalOutput);
    }
    void Warn(sol::this_state State, sol::variadic_args ToPrint) {
        sol::state_view StateView = State;

        auto FinalOutput = ArgsToString(StateView, ToPrint);

        LOG_WARN("{0}", FinalOutput);
    }
    void Error(sol::this_state State, sol::variadic_args ToPrint) {
        sol::state_view StateView = State;

        auto FinalOutput = ArgsToString(StateView, ToPrint);

        LOG_ERROR("{0}", FinalOutput);
    }
    void Critical(sol::this_state State, sol::variadic_args ToPrint) {
        sol::state_view StateView = State;

        auto FinalOutput = ArgsToString(StateView, ToPrint);

        LOG_CRITICAL("{0}", FinalOutput);
    }

    std::string ArgsToString(sol::state_view L, sol::variadic_args ToPrint) {
        auto ToString = L["tostring"];

        std::string FinalOutput = "";
        int i = 0;
        for(auto Print : ToPrint) {
            if(i != 0)
                FinalOutput += "\t";
            FinalOutput += ToString(Print);

            i++;
        }

        return FinalOutput;
    }
}// namespace engine::lualib
