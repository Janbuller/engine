#pragma once

#include "engine/Base.h"
#include "engine/Camera.h"
#include "engine/Scene.h"
#include "engine/systems/ISystem.h"

namespace engine::systems {
  class ModelRenderer : public ISystem {
  public: 
    void Init(sptr<Scene> Scene) override;
    void Update(sptr<Scene> Scene, double DeltaTime) override {};
    void Exit(sptr<Scene> Scene) override {}
    
  public:
    void Render(sptr<Scene> Scene, glm::mat4 View, glm::mat4 Projection);
  };
}
