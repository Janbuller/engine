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

        GLuint GFBO;
        GLuint GPos, GNorm, GCol;
        GLuint DepthRBO;
        GLuint QuadVAO, QuadVBO;

        glcore::Shader GPassShader{"res/Internal/Shaders/GPass.vert", "res/Internal/Shaders/GPass.frag"};
        glcore::Shader DefaultLightingShader{"res/Internal/Shaders/DefaultLightingShader.vert", "res/Internal/Shaders/DefaultLightingShader.frag"};

    public:
        void Init(sptr<Scene> Scene) override;
        void Update(sptr<Scene> Scene, double DeltaTime) override{};
        void Exit(sptr<Scene> Scene) override {}

        void EntityUpdated(sptr<Scene> Scene, Entity Ent) override{}
        void AnyEntityUpdated(sptr<Scene> Scene, Entity Ent) override{}

    public:
        void Render(sptr<Scene> Scene, int Width, int Height);
        void Resize(int Width, int Height);

    private:
        void CreateFramebuffer();
        void CreateScreenQuad();
    };
}// namespace engine::systems
