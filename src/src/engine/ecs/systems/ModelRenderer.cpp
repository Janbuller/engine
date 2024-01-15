#include "engine/ecs/systems/ModelRenderer.h"
#include "engine/ecs/SceneView.h"
#include "engine/ecs/components/Camera.h"
#include "engine/ecs/components/Light.h"
#include "engine/ecs/components/Model.h"
#include "engine/lighting/ShaderLight.h"
#include "engine/ressources/RessourceManager.h"
#include <glad/glad.h>

namespace engine::systems {
    void ModelRenderer::Init(sptr<Scene> Scene) {
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glFrontFace(GL_CW);

        glEnable(GL_MULTISAMPLE);

        glEnable(GL_FRAMEBUFFER_SRGB);

        glGenBuffers(1, &LightSSBO);

        Skybox                               = RessourceManager::Get<Model>("res/Internal/Models/Skybox.obj");
        Skybox.Meshes[0].MeshMaterial.Shader = glcore::Shader{"res/Internal/Shaders/Skybox.vert", "res/Internal/Shaders/Skybox.frag"};

        CreateFramebuffer();

        CreateScreenQuad();
    }

    void ModelRenderer::Render(sptr<Scene> Scene, int Width, int Height) {
        // BACKGROUND
        // ==========
        // Gets camera component of the main scene camera.
        auto &CC = Scene->GetComponent<Camera>(Scene->MainCam.Id);

        // Get the projection and view matrix, which will be used for skybox
        // rendering and later for rendering of all other entities.
        auto Projection = Camera::GetProjectionMatrix(Scene, Scene->MainCam.Id, Width, Height);
        auto View       = Camera::GetViewMatrix(Scene, Scene->MainCam.Id);

        // Set and clear background color
        if (CC.BackgroundColor) {
            auto &BG = CC.BackgroundColor.value();
            glClearColor(BG.r, BG.g, BG.b, BG.a);
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // GEOMETRY PASS
        // =============
        // Bind the GeometryPass framebuffer to draw to.
        glBindFramebuffer(GL_FRAMEBUFFER, GFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw skybox if existing
        if (CC.Skybox) {
            // Get the texture and material
            auto &SkyboxTex = CC.Skybox.value();
            auto &Mat       = Skybox.Meshes[0].MeshMaterial;

            // Bind the matrices to the shader.
            // For the view matrix, the translation is removed by doing
            // glm::mat4{glm::mat3{View}}.
            Mat.Shader.Bind();
            auto SkyboxView = glm::mat4{glm::mat3{View}};
            Mat.Shader.SetMat4("View", SkyboxView);
            Mat.Shader.SetMat4("Projection", Projection);

            // Render the actual skybox
            glDepthMask(GL_FALSE);
            glDisable(GL_CULL_FACE);

            glBindVertexArray(Skybox.Meshes[0].VAO);
            Mat.Shader.SetInt("Skybox", 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, SkyboxTex.Handle);
            glDrawElements(GL_TRIANGLES, Skybox.Meshes[0].Indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            glDepthMask(GL_TRUE);
            glEnable(GL_CULL_FACE);
        }


        // ENTITY RENDERING
        // ----------------
        // Loop through all the entities affected by the ModelRenderer system.
        for (const auto &Entity : Entities) {
            // Grab the transform and model components.
            const auto &ET = Scene->GetComponent<Transform>(Entity);
            const auto &EM = Scene->GetComponent<Model>(Entity);

            // Ignore object if invisible.
            if (!EM.Visible) {
                continue;
            }

            // Grab the transformation (model) matrix.
            auto Model = ET.GetTransformMatrix();

            // Loop through all the meshes in the current model.
            for (auto &Mesh : EM.Meshes) {
                auto &Mat      = Mesh.MeshMaterial;
                auto &Textures = Mat.Textures;

                // Set the MVP-matrices and the main camera position uniforms
                // in the shader.
                GPassShader.Bind();
                GPassShader.SetMat4("View", View);
                GPassShader.SetMat4("Projection", Projection);
                GPassShader.SetMat4("Model", Model);

                // Holds the amount of bound textures of each different type,
                // starting from one. This is used for variable name in
                // shaders.
                std::array<int, (int) TextureType::NONE> TextureTypeCounter{};
                TextureTypeCounter.fill(1);

                // Loop through all mesh textures, generating names and binding
                // them.
                for (unsigned int i = 0; i < Textures.size(); i++) {
                    glActiveTexture(GL_TEXTURE0 + i);

                    TextureType Type = Textures[i].Type;

                    // Get the texture shadername from the TextureInfo array
                    // using the type.
                    std::string TypeName = Material::TextureInfo[(int) Type].ShaderName;

                    // Get the count of the current texturetype and add one to
                    // it.
                    std::string TextureTypeCount = std::to_string(TextureTypeCounter[(int) Type]++);

                    std::string ShaderName = TypeName + TextureTypeCount;

                    GPassShader.SetInt(("MeshMat." + ShaderName).c_str(), i);
                    glBindTexture(GL_TEXTURE_2D, Textures[i].Texture.Handle);
                }
                glActiveTexture(GL_TEXTURE0);

                // Bind the vao and draw.
                glBindVertexArray(Mesh.VAO);
                glDrawElements(GL_TRIANGLES, Mesh.Indices.size(), GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
            }
        }

        // Unbind GPass framebuffer.
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // LIGHTING PASS
        // =============

        // Generate vector of ShaderLights from all light components in scene
        // using SceneView.
        std::vector<lighting::ShaderLight> Lights;

        auto LightsView = SceneView<Transform, Light>{Scene};
        for (const auto &L : LightsView) {
            // Create new empty shaderlight, to be filled in by the current
            // light entity.
            lighting::ShaderLight Light{};

            // Get Transform- and Light-component from current entity.
            auto &LT = Scene->GetComponent<Transform>(L);
            auto &LL = Scene->GetComponent<engine::components::Light>(L);

            // Generate the ShaderLight position variable. The shader uses the
            // w-component of the position vector to determine the light-type.
            // It also expects the xyz-components for a dirlight to be a
            // vector, pointing the direction.
            if (LL.Type == Light::LightType::PointLight) {
                // If it is a pointlight, just grab the position.
                Light.Position = glm::vec4(LT.Position, 1.0f);
            } else if (LL.Type == Light::LightType::DirectionalLight) {
                // If it is a dirlight, rotate a forward facing vector by the
                // light quaternion.
                glm::vec3 Forward = glm::rotate(glm::inverse(LT.Rotation), glm::vec3(0.0, 0.0, -1.0));
                Light.Position    = glm::vec4{Forward, 0.0f};
            }

            // Grab the rest of the variables straight from the light
            // component.
            Light.Color     = LL.Color;
            Light.Intensity = LL.Intensity;

            Light.Constant  = LL.Constant;
            Light.Linear    = LL.Linear;
            Light.Quadratic = LL.Quadratic;

            Lights.push_back(Light);
        }

        // Bind the shader for the lighting pass
        DefaultLightingShader.Bind();

        // Bind the textures of the GPass framebuffer.
        DefaultLightingShader.SetInt("GPos", 0);
        glBindTextureUnit(0, GPos);
        DefaultLightingShader.SetInt("GNorm", 1);
        glBindTextureUnit(1, GNorm);
        DefaultLightingShader.SetInt("GCol", 2);
        glBindTextureUnit(2, GCol);

        DefaultLightingShader.SetVec3("MainCam.CamPos", Scene->GetComponent<Transform>(Scene->MainCam.Id).Position);

        // Generate the light SSBO, which holds all the lights in the scene.
        // SSBO is used for the variable array size feature.
        glNamedBufferData(LightSSBO, sizeof(lighting::ShaderLight) * Lights.size(), Lights.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, LightSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        // Draw the ScreenSpace quad to the screen.
        glBindVertexArray(QuadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0+1);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0+2);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void ModelRenderer::Resize(int Width, int Height) {
        // Resize the textures and renderbuffer for the GPass frambebuffer.
        glBindTexture(GL_TEXTURE_2D, GPos);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Width, Height, 0, GL_RGBA, GL_FLOAT, NULL);
        glBindTexture(GL_TEXTURE_2D, GNorm);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Width, Height, 0, GL_RGBA, GL_FLOAT, NULL);
        glBindTexture(GL_TEXTURE_2D, GCol);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_FLOAT, NULL);

        glNamedRenderbufferStorage(DepthRBO, GL_DEPTH_COMPONENT, Width, Height);
    }


    void ModelRenderer::CreateFramebuffer() {

        glCreateFramebuffers(1, &GFBO);

        glCreateTextures(GL_TEXTURE_2D, 1, &GPos);
        glBindTexture(GL_TEXTURE_2D, GPos);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 800, 600, 0, GL_RGBA, GL_FLOAT, NULL);
        glTextureParameteri(GPos, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(GPos, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glNamedFramebufferTexture(GFBO, GL_COLOR_ATTACHMENT0, GPos, 0);

        glCreateTextures(GL_TEXTURE_2D, 1, &GNorm);
        glBindTexture(GL_TEXTURE_2D, GNorm);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 800, 600, 0, GL_RGBA, GL_FLOAT, NULL);
        glTextureParameteri(GNorm, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(GNorm, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glNamedFramebufferTexture(GFBO, GL_COLOR_ATTACHMENT1, GNorm, 0);

        glCreateTextures(GL_TEXTURE_2D, 1, &GCol);
        glBindTexture(GL_TEXTURE_2D, GCol);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTextureParameteri(GCol, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(GCol, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glNamedFramebufferTexture(GFBO, GL_COLOR_ATTACHMENT2, GCol, 0);

        unsigned int attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
        glNamedFramebufferDrawBuffers(GFBO, 3, attachments);

        glCreateRenderbuffers(1, &DepthRBO);
        glNamedRenderbufferStorage(DepthRBO, GL_DEPTH_COMPONENT, 800, 600);
        glNamedFramebufferRenderbuffer(GFBO, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthRBO);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            LOG_ENGINE_ERROR("Incomplete Framebuffer!");
    }

    void ModelRenderer::CreateScreenQuad() {
        // clang-format off
        float ScreenQuadVerts[] = {
            // Positions        // TexCoords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // clang-format on

        // Create buffers for screen quad
        glCreateVertexArrays(1, &QuadVAO);
        glCreateBuffers(1, &QuadVBO);

        glNamedBufferData(QuadVBO, sizeof(ScreenQuadVerts), &ScreenQuadVerts[0], GL_STATIC_DRAW);

        glVertexArrayVertexBuffer(QuadVAO, 0, QuadVBO, 0, 5 * sizeof(float));

        glEnableVertexArrayAttrib(QuadVAO, 0);
        glEnableVertexArrayAttrib(QuadVAO, 1);

        glVertexArrayAttribFormat(QuadVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribFormat(QuadVAO, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float));

        glVertexArrayAttribBinding(QuadVAO, 0, 0);
        glVertexArrayAttribBinding(QuadVAO, 1, 0);
    }
}// namespace engine::systems
