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

        // LIGHTING
        // ========
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
            if (LL.Type == Light::PointLight) {
                // If it is a pointlight, just grab the position.
                Light.Position = glm::vec4(LT.Position, 1.0f);
            } else if (LL.Type == Light::DirectionalLight) {
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

        // Generate the light SSBO, which holds all the lights in the scene.
        // SSBO is used for the variable array size feature.
        glNamedBufferData(LightSSBO, sizeof(lighting::ShaderLight) * Lights.size(), Lights.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, LightSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        // ENTITY RENDERING
        // ================
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
                Mat.Shader.Bind();
                Mat.Shader.SetMat4("View", View);
                Mat.Shader.SetMat4("Projection", Projection);
                Mat.Shader.SetMat4("Model", Model);

                Mat.Shader.SetVec3("MainCam.CamPos", Scene->GetComponent<Transform>(Scene->MainCam.Id).Position);

                // Holds the amount of bound textures of each different type,
                // starting from one. This is used for variable name in
                // shaders.
                std::array<int, (int) TextureType::NONE> TextureTypeCounter{};
                TextureTypeCounter.fill(1);

                // Loop through all mesh textures, generating names and binding
                // them.
                for (unsigned int i = 0; i < Textures.size(); i++) {
                    glActiveTexture(GL_TEXTURE0 + i);

                    TextureType Type = Textures[i].first;

                    // Get the texture shadername from the TextureInfo array
                    // using the type.
                    std::string TypeName = Material::TextureInfo[(int) Type].ShaderName;

                    // Get the count of the current texturetype and add one to
                    // it.
                    std::string TextureTypeCount = std::to_string(TextureTypeCounter[(int) Type]++);

                    std::string ShaderName = TypeName + TextureTypeCount;

                    Mat.Shader.SetInt(("MeshMat." + ShaderName).c_str(), i);
                    glBindTexture(GL_TEXTURE_2D, Textures[i].second.Handle);
                }
                glActiveTexture(GL_TEXTURE0);

                // Bind the vao and draw.
                glBindVertexArray(Mesh.VAO);
                glDrawElements(GL_TRIANGLES, Mesh.Indices.size(), GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
            }
        }
    }
}// namespace engine::systems
