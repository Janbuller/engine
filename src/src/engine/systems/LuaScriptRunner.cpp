#include "engine/systems/LuaScriptRunner.h"
#include "LuaBridge/detail/LuaException.h"
#include "engine/Base.h"
#include "engine/components/ILuaComponent.h"
#include "engine/components/Script.h"
#include "engine/components/Transform.h"
#include <lua.hpp>
#include <LuaBridge/LuaBridge.h>
#include <LuaBridge/detail/LuaRef.h>
#include <LuaBridge/detail/Namespace.h>
#include <LuaBridge/detail/Stack.h>
#include <fstream>
#include <functional>
#include <re2/re2.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeinfo>

namespace engine::systems {

    void LuaScriptRunner::Init(sptr<Scene> Scene) {
        L = luaL_newstate();
        luaL_openlibs(L);

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

        auto L_Entities = luabridge::newTable(L);
        L_Entities["Globals"] = luabridge::newTable(L);

        for (const auto &Entity : Entities) {
            auto &EScript = Scene->GetComponent<Script>(Entity);
            L_Entities[Entity.Id] = luabridge::newTable(L);
            for (int ScriptIdx = 0; ScriptIdx < EScript.ScriptPaths.size(); ScriptIdx++) {
                L_Entities[Entity.Id][ScriptIdx] = luabridge::newTable(L);
            }
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

        // Run the init function in all the scripts.
        for (const auto &Entity : Entities) {
            auto &EScript = Scene->GetComponent<Script>(Entity);

            for (int ScriptIdx = 0; ScriptIdx < EScript.ScriptPaths.size(); ScriptIdx++) {
                lua_getglobal(L, "Entities");
                auto L_EntityScript = luabridge::LuaRef::fromStack(L, -1)[Entity.Id][ScriptIdx];

                luabridge::push(L, Entity.Id);
                lua_setglobal(L, "EntityID");

                luabridge::push(L, ScriptIdx);
                lua_setglobal(L, "ScriptID");

                SetComponentsInLua(Scene, Entity);

                auto L_Init = L_EntityScript["Init"];
                if (!(L_Init == luabridge::Nil())) {
                    try {
                        L_Init();
                    } catch (luabridge::LuaException) {
                        LOG_ENGINE_ERROR("Failed to run initialization function on lua script \"{0}\"", EScript.ScriptPaths[ScriptIdx]);
                    }
                }

                SetComponentsInEngine(Scene, Entity);
            }
        }
    }

    void LuaScriptRunner::Update(sptr<Scene> Scene, double DeltaTime) {
        // Run the update function in all the scripts.
        for (const auto &Entity : Entities) {
            auto &EScript = Scene->GetComponent<Script>(Entity);

            for (int ScriptIdx = 0; ScriptIdx < EScript.ScriptPaths.size(); ScriptIdx++) {
                lua_getglobal(L, "Entities");
                auto L_EntityScript = luabridge::LuaRef::fromStack(L, -1)[Entity.Id][ScriptIdx];

                luabridge::push(L, Entity.Id);
                lua_setglobal(L, "EntityID");

                luabridge::push(L, ScriptIdx);
                lua_setglobal(L, "ScriptID");

                auto L_Update = L_EntityScript["Update"];
                if (!(L_Update == luabridge::Nil())) {
                    try {
                        SetComponentsInLua(Scene, Entity);
                        L_Update(DeltaTime);
                        SetComponentsInEngine(Scene, Entity);
                    } catch (luabridge::LuaException) {
                        LOG_ENGINE_ERROR("Failed to run update function on lua script \"{0}\"", EScript.ScriptPaths[ScriptIdx]);
                    }
                }
            }
        }
    }

    void LuaScriptRunner::Exit(sptr<Scene> Scene) {
        // Run the Exit function in all the scripts.
        for (const auto &Entity : Entities) {
            auto &EScript = Scene->GetComponent<Script>(Entity);

            for (int ScriptIdx = 0; ScriptIdx < EScript.ScriptPaths.size(); ScriptIdx++) {
                lua_getglobal(L, "Entities");
                auto L_EntityScript = luabridge::LuaRef::fromStack(L, -1)[Entity.Id][ScriptIdx];

                luabridge::push(L, Entity.Id);
                lua_setglobal(L, "EntityID");

                luabridge::push(L, ScriptIdx);
                lua_setglobal(L, "ScriptID");

                auto L_Exit = L_EntityScript["Exit"];
                if (!(L_Exit == luabridge::Nil())) {
                    try {
                        L_Exit();
                    } catch (luabridge::LuaException) {
                        LOG_ENGINE_ERROR("Failed to run exit function on lua script \"{0}\"", EScript.ScriptPaths[ScriptIdx]);
                    }
                }

                SetComponentsInEngine(Scene, Entity);
            }
        }

        lua_close(L);
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
        //
        // Examples:
        // x += 5 --> x = x + 5
        // x *= y --> x = x * y
        RE2::GlobalReplace(&contents, "(\\S+)\\s*([^\\s])=\\s*(\\w+)", "\\1 = \\1 \\2 \\3");

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
        auto TransformTable = ET.GetTable(L);

        lua_getglobal(L, "Entities");
        auto L_Entity = luabridge::LuaRef::fromStack(L, -1)[E.Id];

        L_Entity[ET.GetName()] = TransformTable;
    }

    void LuaScriptRunner::SetComponentsInEngine(sptr<Scene> Scene, const Entity &E) {
        auto &ET = Scene->GetComponent<Transform>(E);

        lua_getglobal(L, "Entities");
        auto L_TransformTable = luabridge::LuaRef::fromStack(L, -1)[E.Id][ET.GetName()];
	float tmp = L_TransformTable["Position"]["x"];

	// LOG_ENGINE_ERROR("{0}", tmp);
        ET.SetTable(L_TransformTable);
    }

}// namespace engine::systems
