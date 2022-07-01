#include "engine/ecs/systems/ModelRenderer.h"
#include "engine/ecs/components/Camera.h"
#include "engine/ecs/components/Model.h"
#include "engine/lighting/Light.h"
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

        Skybox = RessourceManager::Get<Model>("res/Internal/Models/Skybox.obj");
        Skybox.Meshes[0].MeshMaterial.Shader = glcore::Shader{"res/Internal/Shaders/Skybox.vert", "res/Internal/Shaders/Skybox.frag"};
    }

    void ModelRenderer::Render(sptr<Scene> Scene, int Width, int Height) {
        Light Lights[2];

        Lights[0] = Light{};
        Lights[0].Position  = {-1, 1, 0, 1};
        Lights[0].Color     = {1, 0.97, 0.94};
        Lights[0].Intensity = 10.0;
        Lights[0].Constant  = 1.0;
        Lights[0].Linear    = 0.35;
        Lights[0].Quadratic = 0.44;

        Lights[1] = Light{};
        Lights[1].Position  = {-0.74, -1.346, -1.75, 0};
        Lights[1].Color     = {0.9, 0.9, 1.0};
        Lights[1].Intensity = 0.2;
        Lights[1].Constant  = 1.0;
        Lights[1].Linear    = 0.35;
        Lights[1].Quadratic = 0.44;

        glNamedBufferData(LightSSBO, sizeof(Lights), Lights, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, LightSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        auto Projection = Camera::GetProjectionMatrix(Scene, Scene->MainCam.Id, Width, Height);
        auto View = Camera::GetViewMatrix(Scene, Scene->MainCam.Id);

        // Get camera component
        auto &CC = Scene->GetComponent<Camera>(Scene->MainCam.Id);

        // Set and clear background color
        if(CC.BackgroundColor) {
            auto& BG = CC.BackgroundColor.value();
            glClearColor(BG.r, BG.g, BG.b, BG.a);
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw skybox if existing
        if(CC.Skybox) {
            auto& SkyboxTex = CC.Skybox.value();
            auto& Mat = Skybox.Meshes[0].MeshMaterial;

            Mat.Shader.Bind();
            auto SkyboxView = glm::mat4{glm::mat3{View}};
            Mat.Shader.SetMat4("view", SkyboxView);
            Mat.Shader.SetMat4("projection", Projection);

            glDepthMask(GL_FALSE);
            glDisable(GL_CULL_FACE);

            glBindVertexArray(Skybox.Meshes[0].VAO);
            Mat.Shader.SetInt("Skybox", 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, SkyboxTex.Handle);
            glDrawElements(GL_TRIANGLES, Skybox.Meshes[0].Indicies.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            glDepthMask(GL_TRUE);
            glEnable(GL_CULL_FACE);
        }

        for (const auto &Entity : Entities) {
            auto &ET = Scene->GetComponent<Transform>(Entity);
            auto &EM = Scene->GetComponent<Model>(Entity);

            if (!EM.Visible) {
                continue;
            }

            auto Model = ET.GetTransformMatrix();

            for (auto &Mesh : EM.Meshes) {
                auto &Mat = Mesh.MeshMaterial;
                auto &Textures = Mat.Textures;

                Mat.Shader.Bind();
                Mat.Shader.SetMat4("view", View);
                Mat.Shader.SetMat4("projection", Projection);
                Mat.Shader.SetMat4("model", Model);

                Mat.Shader.SetVec3("camera.CamPos", Scene->GetComponent<Transform>(Scene->MainCam.Id).Position);

                int DiffuseIdx = 1;
                int SpecularIdx = 1;

                for (unsigned int i = 0; i < Textures.size(); i++) {
                    glActiveTexture(GL_TEXTURE0 + i);

                    std::string number;
                    std::string type = Textures[i].first;

                    if (type == "texture_diffuse") {
                        number = std::to_string(DiffuseIdx++);
                    } else if (type == "texture_specular") {
                        number = std::to_string(SpecularIdx++);
                    }

                    Mat.Shader.SetInt(("material." + type + number).c_str(), i);
                    glBindTexture(GL_TEXTURE_2D, Textures[i].second.Handle);
                }
                glActiveTexture(GL_TEXTURE0);

                glBindVertexArray(Mesh.VAO);
                glDrawElements(GL_TRIANGLES, Mesh.Indicies.size(), GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
            }
        }
    }
}// namespace engine::systems
