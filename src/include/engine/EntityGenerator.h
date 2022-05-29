#pragma once

#include "engine/Entity.h"
#include <vector>
namespace engine {
  class EntityGenerator {
  private:
    EntityID NextId = 0;
    std::vector<EntityID> AvailableIds{};

  public:
    EntityID GetNextId() {
      if(AvailableIds.size() != 0) {
	auto RetId = AvailableIds.back();
	AvailableIds.pop_back();
	return RetId;
      }

      return NextId++;
    }

    void RemoveEntityId(EntityID E) {
      AvailableIds.push_back(E);
    }
  };
}
