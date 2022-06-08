#pragma once

#include "engine/Base.h"
#include "engine/core/Keys.h"
#include "engine/ecs/Scene.h"
#include "engine/ecs/components/Script.h"
#include "engine/ecs/systems/ISystem.h"

// clang-format off
#include <lua.hpp>
#include <LuaBridge/LuaBridge.h>
// clang-format on

namespace engine::systems {
    class LuaScriptRunner : public ISystem {
    private:
        lua_State *L;

    public:
        LuaScriptRunner();

        void InitializeScripting(sptr<Scene> Scene);

        void Init(sptr<Scene> Scene) override;
        void Update(sptr<Scene> Scene, double DeltaTime) override;
        void Exit(sptr<Scene> Scene) override;

        void OnKeyPressed(sptr<Scene> Scene, Keys Key, int Action);

        template<class R, class... Args>
        void AddLuaFunction(std::function<R(Args...)> Func, std::string NS, std::string Name) {
            luabridge::getGlobalNamespace(L)
                    .beginNamespace(NS.c_str())
                    .addFunction(Name.c_str(), Func)
                    .endNamespace();
        }

    private:
        bool CheckLua(lua_State *L, int r);
        std::string LoadAndReplaceFile(std::string filename);

        void SetComponentsInLua(sptr<Scene> Scene, const Entity &E);
        void SetComponentsInEngine(sptr<Scene> Scene, const Entity &E);

        void SetupGettersAndSetters(sptr<Scene> Scene);

        template<typename... Arguments>
        void RunFunctionForAll(sptr<Scene> Scene, std::string Function, Arguments &&...Args);
    };
}// namespace engine::systems
