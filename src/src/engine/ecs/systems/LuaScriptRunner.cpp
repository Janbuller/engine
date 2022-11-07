#include "engine/ecs/systems/LuaScriptRunner.h"
#include "engine/Base.h"
#include "engine/core/Keys.h"
#include "engine/ecs/SceneView.h"
#include "engine/ecs/components/Camera.h"
#include "engine/ecs/components/Light.h"
#include "engine/ecs/components/Model.h"
#include "engine/ecs/components/Script.h"
#include "engine/ecs/components/Transform.h"
#include "engine/lua/lualib/LEngine.h"
#include "engine/lua/lualib/LInput.h"
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

        auto L_Entities       = L.create_named_table("Entities");
        L_Entities["Globals"] = L.create_table();

        // Creates temporary variables for the GlobalMetaTable Script and
        // EntityCreator Script.
        L["EntityID"] = 0;
        L["ScriptID"] = 0;

        L["Entities"][0]    = L.create_table();
        L["Entities"][0][0] = L.create_table();

        RunScript("res/Internal/Scripts/Entity.lua");

        // Run the GlobalMetaTable Script
        RunScript("res/Internal/Scripts/GlobalMetaTable.lua");

        /* // Create entityobjects for all entities in the scene and make a table */
        /* // for each script. */
        /* auto AllEnt = SceneView<>(Scene); */
        /* for (const auto &Entity : AllEnt) { */
        /*     InitializeEntity(Scene, Entity); */
        /* } */
    }

    void LuaScriptRunner::InitializeInput(glcore::Window *InputWindow) {
        sol::table Engine = L["Engine"].get_or_create<sol::table>();
        lua::lualib::LoadInputLib(Engine, InputWindow);
    }

    void LuaScriptRunner::Init(sptr<Scene> Scene) {
        // Create entityobjects for all entities in the scene and make a table
        // for each script.
        auto AllEnt = SceneView<>(Scene);
        for (const auto &Entity : AllEnt) {
            /* InitializeEntity(Scene, Entity); */
        }
        RunFunctionForAll(Scene, "Init");
    }

    void LuaScriptRunner::Update(sptr<Scene> Scene, double DeltaTime) {
        RunFunctionForAll(Scene, "Update", DeltaTime);
    }

    void LuaScriptRunner::Exit(sptr<Scene> Scene) {
        RunFunctionForAll(Scene, "Exit");
    }

    void LuaScriptRunner::OnKeyPressed(sptr<Scene> Scene, Keys Key, int Action) {
        RunFunctionForAll(Scene, "OnKeyPressed", Key, Action);
    }

    void LuaScriptRunner::AnyEntityUpdated(sptr<Scene> Scene, Entity Ent) {
        SetupComponents(Scene);
        if (Entities.count(Ent)) {
            // Entity was added.
            InitializeEntity(Scene, Ent);
        } else {
            //Entity was removed.
            L["Entities"][Ent.Id] = sol::nil;
        }
    }

    std::string LuaScriptRunner::PreprocessFile(std::string filename) {
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

    void LuaScriptRunner::RunScript(std::string Filename) {
        auto GlobalMetaTableScript = PreprocessFile(Filename);
        L.script(GlobalMetaTableScript, Filename);
    }

    void LuaScriptRunner::InitializeEntity(sptr<Scene> Scene, Entity Entity) {
        // Get the lua function for making a new entity
        auto L_NewEntity = L["NewEntity"];

        // Create entityobjects for all entities in the scene and make a table
        // for each script.
        L["Entities"][Entity.Id]               = L_NewEntity(Entity.Id);
        L["Entities"][Entity.Id]["EntGlobals"] = L.create_table();

        // If the entity has a script component, create a table for all
        // scripts.
        if (Scene->HasComponent<Script>(Entity)) {
            auto &EScript = Scene->GetComponent<Script>(Entity);
            for (int ScriptIdx = 0; ScriptIdx < EScript.ScriptPaths.size(); ScriptIdx++) {
                L["Entities"][Entity.Id][ScriptIdx] = L.create_table();

                const auto &Path = EScript.ScriptPaths[ScriptIdx];

                L["EntityID"] = Entity.Id;
                L["ScriptID"] = ScriptIdx;

                RunScript(Path);
            }
        }
    }

    void LuaScriptRunner::SetupComponents(sptr<Scene> Scene) {
        auto Components = L.create_named_table("Components");
        SetupComponent<Transform>(Scene, "Transform");
        SetupComponent<Model>(Scene, "Model");
        SetupComponent<Light>(Scene, "Light");
        SetupComponent<Camera>(Scene, "Camera");
    }


    template<typename... Arguments>
    void LuaScriptRunner::RunFunctionForAll(sptr<Scene> Scene, std::string Function, Arguments... Args) {
        for (const auto &Entity : Entities) {
            auto &EScript = Scene->GetComponent<Script>(Entity);

            for (int ScriptIdx = 0; ScriptIdx < EScript.ScriptPaths.size(); ScriptIdx++) {
                auto L_EntityScript = L["Entities"][Entity.Id][ScriptIdx];

                L["EntityID"] = Entity.Id;
                L["ScriptID"] = ScriptIdx;

                sol::function L_Func = L_EntityScript[Function];
                if (L_Func.valid()) {
                    L_Func(Args...);
                }
            }
        }
    }

}// namespace engine::systems
