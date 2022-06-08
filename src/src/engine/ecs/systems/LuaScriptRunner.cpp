#include "engine/ecs/systems/LuaScriptRunner.h"
#include "LuaBridge/detail/LuaRef.h"
#include "engine/Base.h"
#include "engine/core/Keys.h"
#include "engine/ecs/components/Model.h"
#include "engine/ecs/components/Script.h"
#include "engine/ecs/components/Transform.h"
#include <fstream>
#include <functional>

// clang-format off
#include <lua.hpp>
#include <LuaBridge/LuaBridge.h>
// clang-format on

#include <re2/re2.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeinfo>

namespace engine::systems {

    LuaScriptRunner::LuaScriptRunner() {
        L = luaL_newstate();
        luaL_openlibs(L);
    }

    void LuaScriptRunner::InitializeScripting(sptr<Scene> Scene) {

        std::function<void(std::string)> TraceFn = [](std::string ToPrint) { LOG_TRACE(ToPrint); };
        std::function<void(std::string)> InfoFn = [](std::string ToPrint) { LOG_INFO(ToPrint); };
        std::function<void(std::string)> WarnFn = [](std::string ToPrint) { LOG_WARN(ToPrint); };
        std::function<void(std::string)> ErrorFn = [](std::string ToPrint) { LOG_ERROR(ToPrint); };
        std::function<void(std::string)> CriticalFn = [](std::string ToPrint) { LOG_CRITICAL(ToPrint); };

        luabridge::getGlobalNamespace(L)
                .beginNamespace("Log")
                .addFunction("Trace", TraceFn)
                .addFunction("Info", InfoFn)
                .addFunction("Warn", WarnFn)
                .addFunction("Error", ErrorFn)
                .addFunction("Critical", CriticalFn)
                .endNamespace();

        SetupGettersAndSetters(Scene);

        auto L_Entities = luabridge::newTable(L);
        L_Entities["Globals"] = luabridge::newTable(L);

        // auto L_NewEntity = luabridge::getGlobal(L, "NewEntity");

        auto EntityScript = LoadAndReplaceFile("res/scripts/internal/Entity.lua");
        if (!CheckLua(L, luaL_dostring(L, EntityScript.c_str()))) {
            LOG_ENGINE_ERROR("Failed to create the entity creator!");
            throw std::runtime_error("Failed to create the entity creator!");
        }

        lua_getglobal(L, "NewEntity");
        auto L_NewEntity = luabridge::LuaRef::fromStack(L, -1);
        lua_pop(L, -1);

        for (const auto &Entity : Entities) {
            auto &EScript = Scene->GetComponent<Script>(Entity);

            try {
                L_Entities[Entity.Id] = L_NewEntity(Entity.Id);
            } catch (luabridge::LuaException e) {
                LOG_ENGINE_ERROR("Lua Error: {}", e.what());
            }

            for (int ScriptIdx = 0; ScriptIdx < EScript.ScriptPaths.size(); ScriptIdx++) {
                L_Entities[Entity.Id][ScriptIdx] = luabridge::newTable(L);
            }
        }

        auto Classes = LoadAndReplaceFile("res/scripts/internal/Classes.lua");
        if (!CheckLua(L, luaL_dostring(L, Classes.c_str()))) {
            LOG_ENGINE_ERROR("Failed to load lua classes!");
            throw std::runtime_error("Failed to load lua classes!");
        }

        luabridge::push(L, L_Entities);
        lua_setglobal(L, "Entities");

        luabridge::push(L, 0);
        lua_setglobal(L, "EntityID");

        luabridge::push(L, 0);
        lua_setglobal(L, "ScriptID");

        auto GlobalMetaTable = LoadAndReplaceFile("res/scripts/internal/GlobalMetaTable.lua");
        if (!CheckLua(L, luaL_dostring(L, GlobalMetaTable.c_str()))) {
            LOG_ENGINE_ERROR("Failed to set the global lua metatable!");
            throw std::runtime_error("Failed to set the global lua metatable!");
        }

        // Run through all scripts and initialize the semi-global
        // variables and functions.
        for (const auto &Entity : Entities) {
            auto &EScript = Scene->GetComponent<Script>(Entity);
            for (int ScriptIdx = 0; ScriptIdx < EScript.ScriptPaths.size(); ScriptIdx++) {
                const auto &Path = EScript.ScriptPaths[ScriptIdx];

                auto ScriptSrc = LoadAndReplaceFile(Path);
                // std::cout << ScriptSrc << std::endl;
                // throw std::runtime_error("lsdjf");

                luabridge::push(L, Entity.Id);
                lua_setglobal(L, "EntityID");

                luabridge::push(L, ScriptIdx);
                lua_setglobal(L, "ScriptID");

                if (!CheckLua(L, luaL_dostring(L, ScriptSrc.c_str()))) {
                    LOG_ENGINE_ERROR("Failed to execute the lua script (\"{0}\")", Path);
                    throw std::runtime_error("Failed to execute lua script.");
                }
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

        lua_close(L);
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
        // lua-supported version.

        // Examples:
        // x += 5 --> x = x + 5
        // x *= y --> x = x * y
        RE2::GlobalReplace(&contents, "(\\S+)\\s*([^\\s=~])=\\s*(\\w+)", "\\1 = \\1 \\2 \\3");

        // Replaces the keyword global into "g_". This allows for using the
        // word "global" to represent global variabls, while lua just sees
        // it as a variable name prefixed with "g_".
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
        auto ModelTable = EM.GetTable(L);

        lua_getglobal(L, "Entities");
        auto L_Entity = luabridge::LuaRef::fromStack(L, -1)[E.Id];
        lua_pop(L, -1);

        L_Entity[ET.GetName()] = TransformTable;
        L_Entity[EM.GetName()] = ModelTable;
    }

    void LuaScriptRunner::SetComponentsInEngine(sptr<Scene> Scene, const Entity &E) {
        auto &ET = Scene->GetComponent<Transform>(E);
        auto &EM = Scene->GetComponent<Model>(E);

        lua_getglobal(L, "Entities");
        auto Entity = luabridge::LuaRef::fromStack(L, -1)[E.Id];
        lua_pop(L, -1);

        auto L_TransformTable = Entity[ET.GetName()];
        auto L_ModelTable = Entity[EM.GetName()];

        // LOG_ENGINE_ERROR("{0}", tmp);
        ET.SetTable(L_TransformTable);
        EM.SetTable(L_ModelTable);
    }

    void LuaScriptRunner::SetupGettersAndSetters(sptr<Scene> Scene) {
        std::function<luabridge::LuaRef(int)> GetTransformFn = [Scene, this](int EID) {
            return Scene->GetComponent<Transform>((EntityID) EID).GetTable(L);
        };

        std::function<void(int, luabridge::LuaRef)> SetTransformFn = [Scene](int EID, luabridge::LuaRef Component) {
            Scene->GetComponent<Transform>((EntityID) EID).SetTable(Component);
        };

        std::function<luabridge::LuaRef(int)> GetModelFn = [Scene, this](int EID) {
            return Scene->GetComponent<Model>((EntityID) EID).GetTable(L);
        };

        std::function<void(int, luabridge::LuaRef)> SetModelFn = [Scene](int EID, luabridge::LuaRef Component) {
            Scene->GetComponent<Model>((EntityID) EID).SetTable(Component);
        };

        luabridge::getGlobalNamespace(L)
                .beginNamespace("Components")
                .addFunction("GetTransform", GetTransformFn)
                .addFunction("SetTransform", SetTransformFn)
                .addFunction("GetModel", GetModelFn)
                .addFunction("SetModel", SetModelFn)
                .endNamespace();
    }


    template<typename... Arguments>
    void LuaScriptRunner::RunFunctionForAll(sptr<Scene> Scene, std::string Function, Arguments &&...Args) {
        for (const auto &Entity : Entities) {
            auto &EScript = Scene->GetComponent<Script>(Entity);

            for (int ScriptIdx = 0; ScriptIdx < EScript.ScriptPaths.size(); ScriptIdx++) {
                lua_getglobal(L, "Entities");
                auto L_EntityScript = luabridge::LuaRef::fromStack(L, -1)[Entity.Id][ScriptIdx];
                lua_pop(L, -1);

                luabridge::push(L, Entity.Id);
                lua_setglobal(L, "EntityID");
                luabridge::push(L, ScriptIdx);
                lua_setglobal(L, "ScriptID");

                auto L_Func = L_EntityScript[Function];
                if (!(L_Func == luabridge::Nil())) {
                    try {
                        L_Func(Args...);
                    } catch (luabridge::LuaException e) {
                        LOG_ENGINE_ERROR("Failed to run function on lua script \"{0}\"", EScript.ScriptPaths[ScriptIdx]);
                        LOG_ENGINE_ERROR("Lua Error: {}", e.what());
                    }
                }
            }
        }
    }

}// namespace engine::systems
