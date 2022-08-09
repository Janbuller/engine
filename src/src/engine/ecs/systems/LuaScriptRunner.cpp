#include "engine/ecs/systems/LuaScriptRunner.h"
#include "engine/Base.h"
#include "engine/core/Keys.h"
#include "engine/ecs/components/Camera.h"
#include "engine/ecs/components/Light.h"
#include "engine/ecs/components/Model.h"
#include "engine/ecs/components/Script.h"
#include "engine/ecs/components/Transform.h"
#include "engine/lua/lualib/LEngine.h"
#include <fstream>
#include <functional>
#include <re2/re2.h>
#include <sol/types.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeinfo>

#include "engine/lua/usertypes/LAllUsertypes.h"

namespace engine::systems {

    LuaScriptRunner::LuaScriptRunner() {
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
        // Load the library of functions for the engine and initialize the
        // usertypes for all the different used classes.
        lua::lualib::LoadEngineLib(L);
        lua::usertypes::InitializeAllUsertypes(L);

        SetupGettersAndSetters(Scene);

        auto L_Entities       = L.create_named_table("Entities");
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

        return contents;
    }

    void LuaScriptRunner::SetupGettersAndSetters(sptr<Scene> Scene) {
        std::function<Transform&(int)> GetTransformFn = [Scene, this](int EID) -> Transform& {
            return Scene->GetComponent<Transform>((EntityID) EID);
        };

        std::function<Model&(int)> GetModelFn = [Scene, this](int EID) -> Model& {
            return Scene->GetComponent<Model>((EntityID) EID);
        };

        std::function<Light&(int)> GetLightFn = [Scene, this](int EID) -> Light& {
            return Scene->GetComponent<Light>((EntityID) EID);
        };

        std::function<Camera&(int)> GetCameraFn = [Scene, this](int EID) -> Camera& {
            return Scene->GetComponent<Camera>((EntityID) EID);
        };

        {
            auto Components            = L["Components"].get_or_create<sol::table>();
            Components["GetTransform"] = GetTransformFn;
            Components["GetModel"]     = GetModelFn;
            Components["GetLight"]     = GetLightFn;
            Components["GetCamera"]    = GetCameraFn;
        }
    }


    template<typename... Arguments>
    void LuaScriptRunner::RunFunctionForAll(sptr<Scene> Scene, std::string Function, Arguments... Args) {
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
