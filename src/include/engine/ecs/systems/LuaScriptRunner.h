#pragma once

#include "engine/Base.h"
#include "engine/core/Keys.h"
#include "engine/ecs/Scene.h"
#include "engine/ecs/components/Script.h"
#include "engine/ecs/systems/ISystem.h"
#include <sol/sol.hpp>

namespace engine::systems {
    class LuaScriptRunner : public ISystem {
    private:
        sol::state L;

    public:
        LuaScriptRunner();

        void InitializeScripting(sptr<Scene> Scene);

        void Init(sptr<Scene> Scene) override;
        void Update(sptr<Scene> Scene, double DeltaTime) override;
        void Exit(sptr<Scene> Scene) override;

        void OnKeyPressed(sptr<Scene> Scene, Keys Key, int Action);

        template<class R, class... Args>
        void AddLuaFunction(std::function<R(Args...)> Func, std::string NS, std::string Name) {
            auto Components  = L[NS].get_or_create<sol::table>();
            Components[Name] = Func;
        }

    private:
        std::string LoadAndReplaceFile(std::string filename);

        void SetComponentsInLua(sptr<Scene> Scene, const Entity &E);
        void SetComponentsInEngine(sptr<Scene> Scene, const Entity &E);

        void SetupGettersAndSetters(sptr<Scene> Scene);

        template<typename... Arguments>
        void RunFunctionForAll(sptr<Scene> Scene, std::string Function, Arguments... Args);
    };
}// namespace engine::systems
