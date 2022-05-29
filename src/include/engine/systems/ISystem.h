#pragma once

#include "engine/Entity.h"
#include "engine/Scene.h"
#include <set>

namespace engine {
  class Scene;
    namespace systems {
        class ISystem {
        public:
	  std::set<Entity> Entities;

            virtual void Init() = 0;
            virtual void Update(sptr<Scene> Scene, double DeltaTime) = 0;
            virtual void Exit() = 0;
        };
    }// namespace systems
}// namespace engine
