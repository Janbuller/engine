#pragma once

#include "Entity.h"
#include "engine/Logger.h"
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace engine {

    struct IComponentVector {
        virtual void TryRemoveComponent(Entity E) {}
    };

    template<typename T>
    class ComponentVector : public IComponentVector {
    private:
        std::unordered_map<EntityID, int> EntityToComponentIdx;
        std::unordered_map<int, EntityID> ComponentToEntityIdx;

        std::vector<T> Components;

    public:
        void AddComponent(Entity E) {
            if (EntityToComponentIdx.find(E.Id) != EntityToComponentIdx.end()) {
                LOG_ENGINE_ERROR("Failed to remove add an already existing \"{0}\" component to an entity with id \"{1}\".", typeid(T).name(), E.Id);
                throw std::invalid_argument("Tried to add existing component to entity");
            }

            // Update the conversion maps.
            EntityToComponentIdx.insert_or_assign(E.Id, Components.size());
            ComponentToEntityIdx.insert_or_assign(Components.size(), E.Id);

            Components.push_back(T{});
        }

        void AddComponent(Entity E, T Component) {
            if (EntityToComponentIdx.find(E.Id) != EntityToComponentIdx.end()) {
                LOG_ENGINE_ERROR("Failed to remove add an already existing \"{0}\" component to an entity with id \"{1}\".", typeid(T).name(), E.Id);
                throw std::invalid_argument("Tried to add existing component to entity");
            }

            // Update the conversion maps.
            EntityToComponentIdx.insert_or_assign(E.Id, Components.size());
            ComponentToEntityIdx.insert_or_assign(Components.size(), E.Id);

            Components.push_back(Component);
        }

        void RemoveComponent(Entity E) {
            if (EntityToComponentIdx.find(E.Id) == EntityToComponentIdx.end()) {
                LOG_ENGINE_ERROR("Failed to remove a non-existant \"{0}\" component from an entity with id \"{1}\".", typeid(T).name(), E.Id);
                throw std::invalid_argument("Tried to remove non-existant component from entity");
            }

            auto RemoveIdx = EntityToComponentIdx[E.Id];
            auto LastIdx = Components.size() - 1;
            auto LastId = ComponentToEntityIdx[LastIdx];

            // Move the last component to the removed component
            Components[RemoveIdx] = Components.back();

            // Update the conversion maps.
            EntityToComponentIdx[LastId] = RemoveIdx;
            ComponentToEntityIdx[RemoveIdx] = LastId;

            EntityToComponentIdx.erase(E.Id);
            ComponentToEntityIdx.erase(LastIdx);

            Components.pop_back();
        }


        T &GetComponent(Entity E) {
            if (EntityToComponentIdx.find(E.Id) == EntityToComponentIdx.end()) {
                LOG_ENGINE_ERROR("Failed to get a non-existant \"{0}\" component from an entity with id \"{1}\".", typeid(T).name(), E.Id);
                throw std::invalid_argument("Tried to get non-existant component from entity");
            }

            return Components[EntityToComponentIdx[E.Id]];
        }

        T &GetComponent(EntityID E) {
            if (EntityToComponentIdx.find(E) == EntityToComponentIdx.end()) {
                LOG_ENGINE_ERROR("Failed to get a non-existant \"{0}\" component from an entity with id \"{1}\".", typeid(T).name(), E);
                throw std::invalid_argument("Tried to get non-existant component from entity");
            }

            return Components[EntityToComponentIdx[E]];
        }

        void TryRemoveComponent(Entity E) {
            if (EntityToComponentIdx.find(E.Id) != EntityToComponentIdx.end()) {
                RemoveComponent(E);
            }
        }
    };
}// namespace engine
