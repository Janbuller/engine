#pragma once

#include "Entity.h"
#include "Scene.h"
#include "engine/Base.h"
#include "engine/ecs/systems/ISystem.h"
#include <bitset>
#include <stdexcept>
#include <unordered_map>

namespace engine {
    class Scene;

    class SystemHandler {
    private:
        std::unordered_map<std::string, std::bitset<MAX_COMPONENTS>> Signatures{};
        std::unordered_map<std::string, sptr<::engine::systems::ISystem>> Systems{};

    public:
        template<typename T>
        std::shared_ptr<T> RegisterSystem() {
            std::string TypeName = typeid(T).name();

            if (Systems.find(TypeName) != Systems.end()) {
                LOG_ENGINE_ERROR("Failed to register already existing \"{0}\" system.", TypeName);
                throw std::invalid_argument("Tried to register already existing system.");
            }

            auto system = std::make_shared<T>();
            Systems.insert({TypeName, system});

            LOG_ENGINE_TRACE("Registered system \"{0}\"", TypeName);

            return system;
        }

        template<typename T>
        sptr<T> GetSystem() {
            std::string TypeName = typeid(T).name();

            if (Systems.find(TypeName) == Systems.end()) {
                LOG_ENGINE_ERROR("Failed to get non-existant system \"{0}\".", TypeName);
                throw std::invalid_argument("Tried to get non-existant system.");
            }

            return std::static_pointer_cast<T>(Systems[TypeName]);
        }


        template<typename T>
        void SetSignature(std::bitset<MAX_COMPONENTS> signature) {
            std::string TypeName = typeid(T).name();

            if (Systems.find(TypeName) == Systems.end()) {
                LOG_ENGINE_ERROR("Failed to set signature of non-existant system \"{0}\"", TypeName);
                throw std::invalid_argument("Tried to set signature of non-existant system.");
            }

            Signatures.insert({TypeName, signature});
        }

        void RemoveEntity(Entity entity) {
            for (const auto &pair : Systems) {
                const auto &System = pair.second;
                System->Entities.erase(entity);
            }
        }

        void EntitySignatureChanged(sptr<Scene> Scene, Entity entity, std::bitset<MAX_COMPONENTS> entitySignature) {
            for (const auto &pair : Systems) {
                const auto &TypeName        = pair.first;
                const auto &System          = pair.second;
                const auto &SystemSignature = Signatures[TypeName];

                System->AnyEntityUpdated(Scene, entity);
                if ((entitySignature & SystemSignature) == SystemSignature) {
                    if(!System->Entities.count(entity)) {
                        System->Entities.insert(entity);
                        System->EntityUpdated(Scene, entity);
                    }
                } else {
                    if(System->Entities.count(entity)) {
                        System->Entities.erase(entity);
                        System->EntityUpdated(Scene, entity);
                    }
                }
            }
        }

        void InitSystems(sptr<Scene> Scene) {
            for (auto const &pair : Systems) {
                const auto &System = pair.second;
                System->Init(Scene);
            }
        }

        void UpdateSystems(sptr<Scene> Scene, double DeltaTime) {
            for (auto const &pair : Systems) {
                const auto &System = pair.second;
                System->Update(Scene, DeltaTime);
            }
        }

        void ExitSystems(sptr<Scene> Scene) {
            for (auto const &pair : Systems) {
                const auto &System = pair.second;
                System->Exit(Scene);
            }
        }
    };
}// namespace engine
