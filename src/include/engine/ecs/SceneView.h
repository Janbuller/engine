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
            unsigned int EntityIdx;
            std::bitset<MAX_COMPONENTS> Signature;

            Iterator(sptr<Scene> S, unsigned int EntityIdx, std::bitset<MAX_COMPONENTS> Signature) : S{S}, EntityIdx{EntityIdx}, Signature{Signature} {}

            Entity operator*() const {
                return S->Entities[EntityIdx];
            }

            bool operator==(const Iterator &other) const {
                return EntityIdx == other.EntityIdx;
            }

            bool operator!=(const Iterator &other) const {
                return EntityIdx != other.EntityIdx;
            }

            Iterator &operator++() {
                do {
                    EntityIdx++;
                } while (EntityIdx < S->Entities.size() && (Signature & S->Entities[EntityIdx].ComponentSignature) != Signature);

                return *this;
            }
        };

        const Iterator begin() const {
            int StartIdx = 0;
            while (StartIdx < S->Entities.size() && (ComponentSignature & S->Entities[StartIdx].ComponentSignature) != ComponentSignature) {
                StartIdx++;
            }

            return Iterator(S, StartIdx, ComponentSignature);
        }

        const Iterator end() const {
            return Iterator(S, S->Entities.size(), ComponentSignature);
        }
    };
}// namespace engine
