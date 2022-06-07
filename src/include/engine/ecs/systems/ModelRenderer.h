#pragma once

#include "engine/Base.h"
#include "engine/ecs/Scene.h"
#include "ISystem.h"

namespace engine::systems {
  class ModelRenderer : public ISystem {
  public: 
    void Init(sptr<Scene> Scene) override;
    void Update(sptr<Scene> Scene, double DeltaTime) override {};
    void Exit(sptr<Scene> Scene) override {}
    
  public:
    void Render(sptr<Scene> Scene, int Width, int Height);
  };
}
