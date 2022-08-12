#pragma once

#include "engine/Base.h"
#include "engine/ecs/Scene.h"
#include "engine/ecs/components/Model.h"
#include "engine/ecs/systems/ISystem.h"

namespace engine::systems {
    class ModelRenderer : public ISystem {
    private:
        unsigned int LightSSBO;
        Model Skybox;

    public:
        void Init(sptr<Scene> Scene) override;
        void Update(sptr<Scene> Scene, double DeltaTime) override{};
        void Exit(sptr<Scene> Scene) override {}

        void EntityUpdated(sptr<Scene> Scene, Entity Ent) override{}
    public:
        void Render(sptr<Scene> Scene, int Width, int Height);
    };
}// namespace engine::systems
