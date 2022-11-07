#pragma once

#include "engine/Base.h"
#include "engine/core/Keys.h"
#include "engine/ecs/Scene.h"
#include "engine/ecs/components/Script.h"
#include "engine/ecs/systems/ISystem.h"
#include "engine/glcore/Window.h"
#include <sol/sol.hpp>

namespace engine::systems {
    class LuaScriptRunner : public ISystem {
    private:

    public:
        sol::state L;
        LuaScriptRunner();

        void InitializeScripting(sptr<Scene> Scene);
        void InitializeInput(glcore::Window *InputWindow);

        void Init(sptr<Scene> Scene) override;
        void Update(sptr<Scene> Scene, double DeltaTime) override;
        void Exit(sptr<Scene> Scene) override;

        void OnKeyPressed(sptr<Scene> Scene, Keys Key, int Action);

        void EntityUpdated(sptr<Scene> Scene, Entity Ent) override{}
        void AnyEntityUpdated(sptr<Scene> Scene, Entity Ent) override;

        void SetupComponents(sptr<Scene> Scene);

    private:
        std::string PreprocessFile(std::string filename);
        void RunScript(std::string Filename);

        void InitializeEntity(sptr<Scene> Scene, Entity Entity);

        template<typename T>
        void SetupComponent(sptr<Scene> Scene, std::string Name) {
            auto Components = L["Components"].get_or_create<sol::table>();
            Components[Name].get_or_create<sol::table>();

            auto ComponentVec = Scene->GetComponentVector<T>();
            for (const auto [EID, CID] : ComponentVec->EntityToComponentIdx) {
                Components[Name][EID] = &Scene->GetComponent<T>(EID);
            }
        }

        template<typename... Arguments>
        void RunFunctionForAll(sptr<Scene> Scene, std::string Function, Arguments... Args);
    };
}// namespace engine::systems
