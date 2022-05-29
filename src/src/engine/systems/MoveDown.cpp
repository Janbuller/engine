#include "engine/systems/MoveDown.h"
#include "engine/Base.h"
#include "engine/Scene.h"
#include "engine/components/Transform.h"

namespace engine::systems {
  void MoveDown::Update(sptr<Scene> Scene, double DeltaTime) {
    for(const auto& Entity : Entities) {
      auto& ET = Scene->GetComponent<Transform>(Entity);

      ET.Position.y -= DeltaTime;
    }
  }
}
