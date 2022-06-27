#include "engine/ecs/systems/ModelRenderer.h"
#include "engine/ecs/components/Camera.h"
#include "engine/ecs/components/Model.h"
#include "engine/lighting/Light.h"
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
    }

    void ModelRenderer::Render(sptr<Scene> Scene, int Width, int Height) {
        Light Lights[2];
        Lights[0] = Light{
                {-1, 1, 0, 1},
                {1, 0.97, 0.94},
                10.0,
                1.0,
                0.35,
                0.44};

        Lights[1] = Light{
                {-0.74, -1.346, -1.75, 0},
                {0.9, 0.9, 1.0},
                0.2,
                1.0,
                0.35,
                0.44};

        glNamedBufferData(LightSSBO, sizeof(Lights), Lights, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, LightSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, LightSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        for (const auto &Entity : Entities) {
            auto &ET = Scene->GetComponent<Transform>(Entity);
            auto &EM = Scene->GetComponent<Model>(Entity);

            if (!EM.Visible) {
                continue;
            }

            auto Projection = Camera::GetProjectionMatrix(Scene, Scene->MainCam.Id, Width, Height);
            auto View = Camera::GetViewMatrix(Scene, Scene->MainCam.Id);
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

                    /* LOG_ENGINE_CRITICAL("{0}", Textures[i].second.Handle); */
                }
                glActiveTexture(GL_TEXTURE0);

                glBindVertexArray(Mesh.VAO);
                glDrawElements(GL_TRIANGLES, Mesh.Indicies.size(), GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
            }
        }
    }
}// namespace engine::systems
