#pragma once

#include "ComponentHandler.h"
#include "ComponentVector.h"
#include "Entity.h"
#include "EntityGenerator.h"
#include "SystemHandler.h"
#include "engine/ecs/components/Transform.h"
/* #include "engine/ecs/systems/LuaScriptRunner.h" */
#include <algorithm>
#include <bitset>
#include <memory>
#include <unordered_map>
#include <vector>

namespace engine {
    using namespace components;

    // class ISystem;
    // class SystemHandler;

    class Scene : public std::enable_shared_from_this<Scene> {
    public:
        Entity MainCam;

    public:
        std::unordered_map<int, Entity> Entities;

        EntityGenerator EGen{};

        ComponentHandler Components;
        SystemHandler Systems;

        Entity &AddEntity() {
            auto NewId = EGen.GetNextId();
            LOG_ENGINE_TRACE("Adding Entity with ID {}", NewId);
            Entities.insert({NewId, Entity{NewId, std::bitset<MAX_COMPONENTS>()}});

            Systems.EntitySignatureChanged(shared_from_this(), Entities[NewId], Entities[NewId].ComponentSignature);

            return Entities[NewId];
        }

        void RemoveEntity(Entity E) {
            LOG_ENGINE_TRACE("Removing Entity with ID {}", E.Id);
            Systems.EntitySignatureChanged(shared_from_this(), Entities[E.Id], 0);

            Entities.erase(E.Id);
            EGen.RemoveEntityId(E.Id);

            Components.RemoveEntity(E);
            Systems.RemoveEntity(E);
        }

        bool HasEntity(Entity E) {
            return Entities.count(E.Id);
        }

        template<typename T>
        void RegisterComponentType() {
            Components.RegisterComponentType<T>();
        }

        template<typename T>
        T &AddComponent(Entity &E) {
            auto &NewComponent = Components.AddComponent<T>(E);

            auto &Signature = E.ComponentSignature;
            Signature.set(GetComponentId<T>(), true);

            Systems.EntitySignatureChanged(shared_from_this(), E, Signature);
            return NewComponent;
        }

        template<typename T>
        T &AddComponent(Entity &E, T Component) {
            auto &NewComponent = Components.AddComponent<T>(E, Component);

            auto &Signature = E.ComponentSignature;
            Signature.set(GetComponentId<T>(), true);

            Systems.EntitySignatureChanged(shared_from_this(), E, Signature);
            return NewComponent;
        }

        template<typename T>
        void RemoveComponent(Entity E) {
            Components.RemoveComponent<T>(E);

            auto &Signature = E.ComponentSignature;
            Signature.set(GetComponentId<T>(), false);

            Systems.EntitySignatureChanged(shared_from_this(), E, Signature);
        }

        template<typename T>
        T &GetComponent(Entity E) {
            return Components.GetComponent<T>(E);
        }

        template<typename T>
        T &GetComponent(EntityID E) {
            return Components.GetComponent<T>(E);
        }

        template<typename T>
        bool HasComponent(Entity E) {
            const auto &EntSig      = E.ComponentSignature;
            const auto &ComponentId = GetComponentId<T>();

            return EntSig[ComponentId];
        }

        template<typename T>
        int GetComponentId() {
            return Components.GetComponentId<T>();
        }

        template<typename T>
        sptr<ComponentVector<T>> GetComponentVector() {
            return Components.GetComponentVector<T>();
        }

        template<typename T>
        sptr<T> RegisterSystem() {
            return Systems.RegisterSystem<T>();
        }

        template<typename T>
        sptr<T> GetSystem() {
            return Systems.GetSystem<T>();
        }

        template<typename T>
        void SetSystemSignature(std::bitset<MAX_COMPONENTS> Signature) {
            Systems.SetSignature<T>(Signature);
        }

        void Init() {
            Systems.InitSystems(shared_from_this());
        }

        void Update(double DeltaTime) {
            Systems.UpdateSystems(shared_from_this(), DeltaTime);
        }

        void Exit() {
            Systems.ExitSystems(shared_from_this());
        }
    };
}// namespace engine
