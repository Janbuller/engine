#pragma once

#include "engine/Base.h"
#include "engine/Scene.h"
#include "engine/systems/ISystem.h"

namespace engine::systems {
  class MoveDown : public ISystem {
  public: 
    void Init(sptr<Scene> Scene) override {};
    void Update(sptr<Scene> Scene, double DeltaTime) override;
    void Exit(sptr<Scene> Scene) override {};
  };
}
