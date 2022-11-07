#pragma once

#include "engine/Base.h"
#include "engine/ecs/Scene.h"
#include "engine/ecs/systems/ISystem.h"

namespace engine::systems {
    class MoveDown : public ISystem {
    public:
        void Init(sptr<Scene> Scene) override{};
        void Update(sptr<Scene> Scene, double DeltaTime) override;
        void Exit(sptr<Scene> Scene) override{};

        void EntityUpdated(sptr<Scene> Scene, Entity Ent) override{}
        void AnyEntityUpdated(sptr<Scene> Scene, Entity Ent) override{}
    };
}// namespace engine::systems
