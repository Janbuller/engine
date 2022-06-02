#include "engine/systems/LuaScriptRunner.h"
#include "LuaBridge/detail/LuaException.h"
#include "engine/Base.h"
#include "engine/Keys.h"
#include "engine/components/ILuaComponent.h"
#include "engine/components/Model.h"
#include "engine/components/Script.h"
#include "engine/components/Transform.h"
#include <LuaBridge/LuaBridge.h>
#include <fstream>
#include <functional>
#include <lua.h>
#include <lua.hpp>
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

    void LuaScriptRunner::Init(sptr<Scene> Scene) {
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

        for (const auto &Entity : Scene->Entities) {
            auto &EScript = Scene->GetComponent<Script>(Entity);
            L_Entities[Entity.Id] = luabridge::newTable(L);
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

      RunFunctionForAll(Scene, "Init");
    }

    void LuaScriptRunner::Update(sptr<Scene> Scene, double DeltaTime) {
      for(auto& E : Scene->Entities) {
	SetComponentsInLua(Scene, E);
      }

      RunFunctionForAll(Scene, "Update", DeltaTime);

      for(auto& E : Scene->Entities) {
	SetComponentsInEngine(Scene, E);
      }
    }

    void LuaScriptRunner::Exit(sptr<Scene> Scene) {
      RunFunctionForAll(Scene, "Exit");

        lua_close(L);
    }

  void LuaScriptRunner::OnKeyPressed(sptr<Scene> Scene, Keys Key, int Action) {
    RunFunctionForAll(Scene, "OnKeyPressed", (int)Key, Action);
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
        RE2::GlobalReplace(&contents, "(\\S+)\\s*([^\\s=])=\\s*(\\w+)", "\\1 = \\1 \\2 \\3");

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

}// namespace engine::systems
