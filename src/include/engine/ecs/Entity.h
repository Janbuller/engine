#pragma once

#include <bitset>
namespace engine {
    using EntityID                  = int;
    static const int MAX_COMPONENTS = 32;

    struct Entity {
        EntityID Id;
        std::bitset<MAX_COMPONENTS> ComponentSignature;

        bool operator<(const Entity &Other) const {
            return Id < Other.Id;
        }

        bool operator==(const Entity &Other) const {
            return Id == Other.Id;
        }
    };
}// namespace engine
