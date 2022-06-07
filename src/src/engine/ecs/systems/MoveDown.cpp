#include "engine/ecs/systems/MoveDown.h"
#include "engine/Base.h"
#include "engine/ecs/Scene.h"
#include "engine/ecs/components/Transform.h"

namespace engine::systems {
  void MoveDown::Update(sptr<Scene> Scene, double DeltaTime) {
    for(const auto& Entity : Entities) {
      auto& ET = Scene->GetComponent<Transform>(Entity);

      ET.Position.y -= DeltaTime;
    }
  }
}
