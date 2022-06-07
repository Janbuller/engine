#pragma once

#include <bitset>
namespace engine {
  using EntityID = int;
  static const int MAX_COMPONENTS = 32;

  struct Entity {
    EntityID Id;
    std::bitset<MAX_COMPONENTS> ComponentSignature;

    bool operator< (const Entity& t) const {
      return Id < t.Id;
    }
  };
}
