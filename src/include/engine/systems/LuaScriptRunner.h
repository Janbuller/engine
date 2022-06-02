#pragma once

#include "engine/Base.h"
#include "engine/Keys.h"
#include "engine/Scene.h"
#include "engine/components/Script.h"
#include "engine/systems/ISystem.h"
#include <lua.hpp>
#include <LuaBridge/LuaBridge.h>

namespace engine::systems {
  class LuaScriptRunner : public ISystem {
  private:
    lua_State* L;

  public: 
    LuaScriptRunner();

    void InitializeScripting(sptr<Scene> Scene);
    
    void Init(sptr<Scene> Scene) override;
    void Update(sptr<Scene> Scene, double DeltaTime) override;
    void Exit(sptr<Scene> Scene) override;

    void OnKeyPressed(sptr<Scene> Scene, Keys Key, int Action);

    template< class R, class... Args >
    void AddLuaFunction(std::function<R(Args...)> Func, std::string NS, std::string Name) {
        luabridge::getGlobalNamespace(L)
                .beginNamespace(NS.c_str())
                .addFunction(Name.c_str(), Func)
                .endNamespace();
    }

  private:
    bool CheckLua(lua_State *L, int r);
    std::string LoadAndReplaceFile(std::string filename);

    void SetComponentsInLua(sptr<Scene> Scene, const Entity& E);
    void SetComponentsInEngine(sptr<Scene> Scene, const Entity& E);

    void SetupGettersAndSetters(sptr<Scene> Scene);

    template<typename... Arguments>
    void RunFunctionForAll(sptr<Scene> Scene, std::string Function, Arguments&&... Args) {
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
  };
}
