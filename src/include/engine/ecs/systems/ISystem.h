#pragma once

#include "engine/ecs/Entity.h"
#include "engine/ecs/Scene.h"
#include <set>

namespace engine {
    class Scene;
    namespace systems {
        class ISystem {
        public:
            std::set<Entity> Entities;

            virtual void Init(sptr<Scene> Scene)                     = 0;
            virtual void Update(sptr<Scene> Scene, double DeltaTime) = 0;
            virtual void Exit(sptr<Scene> Scene)                     = 0;
        };
    }// namespace systems
}// namespace engine
