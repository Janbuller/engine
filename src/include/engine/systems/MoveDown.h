#pragma once

#include "engine/Base.h"
#include "engine/Scene.h"
#include "engine/systems/ISystem.h"

namespace engine::systems {
  class MoveDown : public ISystem {
  public: 
    void Init() override {};
    void Update(sptr<Scene> Scene, double DeltaTime) override;
    void Exit() override {};
  };
}
