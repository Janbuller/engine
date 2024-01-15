#pragma once

#include "engine/ecs/Entity.h"
#include "engine/ecs/Scene.h"

namespace engine {
    template<typename... Components>
    class SceneView {
        sptr<Scene> S;
        std::bitset<MAX_COMPONENTS> ComponentSignature;

    public:
        SceneView(sptr<Scene> S) : S{S} {
            int ComponentIds[] = {0, S->GetComponentId<Components>()...};
            for (int i = 1; i < (sizeof...(Components) + 1); i++) {
                ComponentSignature.set(ComponentIds[i]);
            }
        }

        struct Iterator {
            sptr<Scene> S;
            std::unordered_map<int, Entity>::const_iterator EntityIterator;
            std::bitset<MAX_COMPONENTS> Signature;

            Iterator(sptr<Scene> S, std::unordered_map<int, Entity>::const_iterator EntityIterator, std::bitset<MAX_COMPONENTS> Signature)
                : S{S}, EntityIterator{EntityIterator}, Signature{Signature} {}

            Entity operator*() const {
                return EntityIterator->second;
            }

            bool operator==(const Iterator &other) const {
                return EntityIterator == other.EntityIterator;
            }

            bool operator!=(const Iterator &other) const {
                return EntityIterator != other.EntityIterator;
            }

            Iterator &operator++() {
                ++EntityIterator;
                while (EntityIterator != S->Entities.end() && (Signature & EntityIterator->second.ComponentSignature) != Signature) {
                    ++EntityIterator;
                }
                return *this;
            }
        };

        const Iterator begin() const {
            auto entityIterator = S->Entities.begin();
            while (entityIterator != S->Entities.end() && (ComponentSignature & entityIterator->second.ComponentSignature) != ComponentSignature) {
                ++entityIterator;
            }

            return Iterator(S, entityIterator, ComponentSignature);
        }

        const Iterator end() const {
            return Iterator(S, S->Entities.end(), ComponentSignature);
        }
    };
}// namespace engine
