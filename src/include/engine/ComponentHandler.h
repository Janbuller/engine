#pragma once

#include "engine/ComponentVector.h"
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include "engine/Base.h"

namespace engine {
    class ComponentHandler {
    private:
        std::unordered_map<std::string, int> ComponentIds{};

        int NextComponentId = 0;

    public:
        std::unordered_map<std::string, sptr<IComponentVector>> ComponentVectors{};

        template<typename T>
        void RegisterComponentType() {
            const std::string TypeName = typeid(T).name();

            if (ComponentIds.find(TypeName) != ComponentIds.end()) {
                LOG_ENGINE_ERROR("Failed to register already existing \"{0}\" component type.", TypeName);
                throw std::invalid_argument("Tried to register an already existing component type.");
            }

            ComponentIds.insert({TypeName, NextComponentId++});
            ComponentVectors.insert({TypeName, std::make_shared<ComponentVector<T>>()});

            LOG_ENGINE_TRACE("Registered component \"{0}\"", TypeName);
        }

        template<typename T>
        int GetComponentId() {
            const std::string TypeName = typeid(T).name();

            if (ComponentIds.find(TypeName) == ComponentIds.end()) {
                LOG_ENGINE_ERROR("Failed to get component id of non-registered component \"{0}\"", TypeName);
                throw std::invalid_argument("Tried to component id of non-registered component.");
            }

            return ComponentIds[TypeName];
        }

        template<typename T>
        sptr<ComponentVector<T>> GetComponentVector() {
            const std::string TypeName = typeid(T).name();

            if (ComponentIds.find(TypeName) == ComponentIds.end()) {
                LOG_ENGINE_ERROR("Failed to get component vector of non-registered component \"{0}\"", TypeName);
                throw std::invalid_argument("Tried to component vector of non-registered component.");
            }

            return std::static_pointer_cast<ComponentVector<T>>(ComponentVectors[TypeName]);
        }

        template<typename T>
        void AddComponent(Entity E) {
            GetComponentVector<T>()->AddComponent(E);
        }

        template<typename T>
        void AddComponent(Entity E, T Component) {
	  GetComponentVector<T>()->AddComponent(E, Component);
        }

        template<typename T>
        void RemoveComponent(Entity E) {
            GetComponentVector<T>()->RemoveComponent(E);
        }

        template<typename T>
        T &GetComponent(Entity E) {
            return GetComponentVector<T>()->GetComponent(E);
        }

        template<typename T>
        T &GetComponent(EntityID E) {
            return GetComponentVector<T>()->GetComponent(E);
        }

        void RemoveEntity(Entity entity) {
            for (auto const &pair : ComponentVectors) {
                auto const &component = pair.second;

                component->TryRemoveComponent(entity);
            }
        }
    };
}// namespace engine
