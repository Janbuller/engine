#include "engine/ecs/systems/LuaScriptRunner.h"
#include "engine/Base.h"
#include "engine/core/Keys.h"
#include "engine/ecs/components/Model.h"
#include "engine/ecs/components/Script.h"
#include "engine/ecs/components/Transform.h"
#include <fstream>
#include <functional>
#include <re2/re2.h>
#include <sol/types.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeinfo>

namespace engine::systems {

    LuaScriptRunner::LuaScriptRunner() {
        lua_State *Ls = luaL_newstate();
        L.open_libraries(
                sol::lib::base,
                sol::lib::package,
                sol::lib::coroutine,
                sol::lib::string,
                sol::lib::os,
                sol::lib::math,
                sol::lib::table,
                sol::lib::debug,
                sol::lib::bit32,
                sol::lib::io,
                sol::lib::ffi,
                sol::lib::jit);
    }

    void LuaScriptRunner::InitializeScripting(sptr<Scene> Scene) {

        std::function<void(sol::variadic_args)> PrintFn = [](sol::variadic_args ToPrint) {
            std::string FinalString;
            for(auto Print : ToPrint) {
                FinalString += Print;
                FinalString += "\t";
            }
            LOG_TRACE("{}", FinalString);
        };
        L["print"] = PrintFn;

        std::function<void(std::string)> TraceFn    = [](std::string ToPrint) { LOG_TRACE(ToPrint); };
        std::function<void(std::string)> InfoFn     = [](std::string ToPrint) { LOG_INFO(ToPrint); };
        std::function<void(std::string)> WarnFn     = [](std::string ToPrint) { LOG_WARN(ToPrint); };
        std::function<void(std::string)> ErrorFn    = [](std::string ToPrint) { LOG_ERROR(ToPrint); };
        std::function<void(std::string)> CriticalFn = [](std::string ToPrint) { LOG_CRITICAL(ToPrint); };

        {
            auto Log = L["Log"].get_or_create<sol::table>();
            Log["Trace"]    = TraceFn;
            Log["Info"]     = InfoFn;
            Log["Warn"]     = WarnFn;
            Log["Error"]    = ErrorFn;
            Log["Critical"] = CriticalFn;
        }

        SetupGettersAndSetters(Scene);

        auto L_Entities = L.create_named_table("Entities");
        L_Entities["Globals"] = L.create_table();

        auto EntityScript = LoadAndReplaceFile("res/Internal/Scripts/Entity.lua");
        L.script(EntityScript);

        auto L_NewEntity = L["NewEntity"];

        for (const auto &Entity : Entities) {
            auto &EScript = Scene->GetComponent<Script>(Entity);

            L_Entities[Entity.Id] = L_NewEntity(Entity.Id);

            for (int ScriptIdx = 0; ScriptIdx < EScript.ScriptPaths.size(); ScriptIdx++) {
                L_Entities[Entity.Id][ScriptIdx] = L.create_table();
            }
        }

        auto Classes = LoadAndReplaceFile("res/Internal/Scripts/Classes.lua");
        L.script(Classes);

        L["EntityID"] = 0;
        L["ScriptID"] = 0;

        auto GlobalMetaTable = LoadAndReplaceFile("res/Internal/Scripts/GlobalMetaTable.lua");
        L.script(GlobalMetaTable);

        // Run through all scripts and initialize the semi-global
        // variables and functions.
        for (const auto &Entity : Entities) {
            auto &EScript = Scene->GetComponent<Script>(Entity);
            for (int ScriptIdx = 0; ScriptIdx < EScript.ScriptPaths.size(); ScriptIdx++) {
                const auto &Path = EScript.ScriptPaths[ScriptIdx];

                auto ScriptSrc = LoadAndReplaceFile(Path);

                L["EntityID"] = Entity.Id;
                L["ScriptID"] = ScriptIdx;

                L.script(ScriptSrc);
            }
        }
    }

    void LuaScriptRunner::Init(sptr<Scene> Scene) {
        RunFunctionForAll(Scene, "Init");
    }

    void LuaScriptRunner::Update(sptr<Scene> Scene, double DeltaTime) {
        RunFunctionForAll(Scene, "Update", DeltaTime);
    }

    void LuaScriptRunner::Exit(sptr<Scene> Scene) {
        RunFunctionForAll(Scene, "Exit");
    }

    void LuaScriptRunner::OnKeyPressed(sptr<Scene> Scene, Keys Key, int Action) {
        RunFunctionForAll(Scene, "OnKeyPressed", (int) Key, Action);
    }

    bool LuaScriptRunner::CheckLua(lua_State *L, int r) {
        if (r != LUA_OK) {
            std::string errormsg = lua_tostring(L, -1);
            LOG_ENGINE_ERROR(errormsg);
            return false;
        }
        return true;
    }

    std::string LuaScriptRunner::LoadAndReplaceFile(std::string filename) {
        std::ifstream file{filename};
        std::stringstream buffer;
        buffer << file.rdbuf();

        auto contents = buffer.str();

        // Replaces compound assignments (+=, -=, *=, /=) with a
        // lua-supported version. Exempts whitespace, "=" and "~" before the
        // equals sign.
        //
        // Examples:
        // x += 5 --> x = x + 5
        // x *= y --> x = x * y
        //
        // x == y --> x == y    (stays same)
        // x ~= y --> x ~= y    (stays same)
        RE2::GlobalReplace(&contents, "(\\S+)\\s*([^\\s=~])=\\s*(\\w+)", "\\1 = \\1 \\2 \\3");

        // Replaces the keyword global into "g_". This allows for using the
        // word "global" to represent global variabls, while the
        // lua-interpreter just sees it as a variable name prefixed with "g_".
        RE2::GlobalReplace(&contents, "global (\\w+)", "g_\\1");

        // RE2::GlobalReplace(&contents, "(g_)(\\s*[\\n;])", "\\1 = nil\\2");
        // RE2::GlobalReplace(&contents, "global (\\w+)", "GameObject[\"Globals\"][\"\\1\"]");
        // RE2::GlobalReplace(&contents, "(GameObject\\[\"Globals\"\\]\\[.+\\])(\\s*[\\n;])", "\\1 = nil\\2");

        return contents;
    }

    void LuaScriptRunner::SetComponentsInLua(sptr<Scene> Scene, const Entity &E) {
        const auto &ET = Scene->GetComponent<Transform>(E);
        const auto &EM = Scene->GetComponent<Model>(E);
        auto TransformTable = ET.GetTable(L);
        /* auto ModelTable = EM.GetTable(L); */

        auto L_Entity = L["Entities"][E.Id];

        L_Entity[ET.GetName()] = TransformTable;
        /* L_Entity[EM.GetName()] = ModelTable; */
    }

    void LuaScriptRunner::SetComponentsInEngine(sptr<Scene> Scene, const Entity &E) {
        auto &ET = Scene->GetComponent<Transform>(E);
        /* auto &EM = Scene->GetComponent<Model>(E); */

        auto Entity = L["Entities"][E.Id];

        auto L_TransformTable = Entity[ET.GetName()];
        /* auto L_ModelTable = Entity[EM.GetName()]; */

        ET.SetTable(L_TransformTable);
        /* EM.SetTable(L_ModelTable); */
    }

    void LuaScriptRunner::SetupGettersAndSetters(sptr<Scene> Scene) {
        std::function<sol::table(int)> GetTransformFn = [Scene, this](int EID) {
            return Scene->GetComponent<Transform>((EntityID) EID).GetTable(L);
        };

        std::function<void(int, sol::table)> SetTransformFn = [Scene](int EID, sol::table Component) {
            Scene->GetComponent<Transform>((EntityID) EID).SetTable(Component);
        };

        /* std::function<sol::table(int)> GetModelFn = [Scene, this](int EID) { */
        /*     return Scene->GetComponent<Model>((EntityID) EID).GetTable(L); */
        /* }; */

        /* std::function<void(int, sol::table)> SetModelFn = [Scene](int EID, sol::table Component) { */
        /*     Scene->GetComonent<Model>((EntityID) EID).SetTable(Component); */
        /* }; */

        {
            auto Components = L["Components"].get_or_create<sol::table>();
            Components["GetTransform"] = GetTransformFn;
            Components["SetTransform"] = SetTransformFn;
            /* Components["GetModel"]     = GetModelFn; */
            /* Components["SetModel"]     = SetModelFn; */
        }
    }


    template<typename... Arguments>
    void LuaScriptRunner::RunFunctionForAll(sptr<Scene> Scene, std::string Function, Arguments ...Args) {
        for (const auto &Entity : Entities) {
            auto &EScript = Scene->GetComponent<Script>(Entity);

            for (int ScriptIdx = 0; ScriptIdx < EScript.ScriptPaths.size(); ScriptIdx++) {
                auto L_EntityScript = L["Entities"][Entity.Id][ScriptIdx];

                L["EntityID"] = Entity.Id;
                L["ScriptID"] = ScriptIdx;

                auto L_Func = L_EntityScript[Function];
                if (L_Func) {
                    L_Func(Args...);
                }
            }
        }
    }

}// namespace engine::systems
