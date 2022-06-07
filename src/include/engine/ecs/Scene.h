#pragma once

#include "ComponentHandler.h"
#include "ComponentVector.h"
#include "Entity.h"
#include "EntityGenerator.h"
#include "SystemHandler.h"
#include "engine/ecs/components/Transform.h"
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
        std::vector<Entity> Entities;

        EntityGenerator EGen{};

        ComponentHandler Components;
        SystemHandler Systems;

        Entity &AddEntity() {
            auto NewId = EGen.GetNextId();
            Entities.push_back(Entity{NewId, std::bitset<MAX_COMPONENTS>()});

            return Entities.back();
        }

        void RemoveEntity(Entity E) {
            // Entities.erase(std::find(Entities.begin(), Entities.end(), E));
            // EGen.RemoveEntityId(E.Id);
            Components.RemoveEntity(E);
            Systems.RemoveEntity(E);
        }

        template<typename T>
        void RegisterComponentType() {
            Components.RegisterComponentType<T>();
        }

        template<typename T>
        void AddComponent(Entity &E) {
            Components.AddComponent<T>(E);

            auto &Signature = E.ComponentSignature;
            Signature.set(GetComponentId<T>(), true);

            Systems.EntitySignatureChanged(E, Signature);
        }

        template<typename T>
        void AddComponent(Entity &E, T Component) {
            Components.AddComponent<T>(E, Component);

            auto &Signature = E.ComponentSignature;
            Signature.set(GetComponentId<T>(), true);

            Systems.EntitySignatureChanged(E, Signature);
        }

        template<typename T>
        void RemoveComponent(Entity E) {
            Components.RemoveComponent<T>(E);

            auto &Signature = E.ComponentSignature;
            Signature.set(GetComponentId<T>(), false);

            Systems.EntitySignatureChanged(E, Signature);
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
            const auto &EntSig = E.ComponentSignature;
            const auto &ComponentId = GetComponentId<T>();

            return EntSig[ComponentId];
        }

        template<typename T>
        int GetComponentId() {
            return Components.GetComponentId<T>();
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
