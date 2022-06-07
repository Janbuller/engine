#include "engine/ecs/systems/ModelRenderer.h"
#include "engine/ecs/components/Model.h"
#include "engine/ecs/components/Camera.h"
#include <glad/glad.h>

namespace engine::systems {
    void ModelRenderer::Init(sptr<Scene> Scene) {
        glEnable(GL_DEPTH_TEST);

        // glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glFrontFace(GL_CW);

	glEnable(GL_MULTISAMPLE);
    }

    void ModelRenderer::Render(sptr<Scene> Scene, int Width, int Height) {
        for (const auto &Entity : Entities) {
            auto &ET = Scene->GetComponent<Transform>(Entity);
            auto &EM = Scene->GetComponent<Model>(Entity);

	    if(!EM.Visible) {
	      continue;
	    }

            glm::mat4 Projection = Camera::GetProjectionMatrix(Scene, Scene->MainCam.Id, Width, Height);
            glm::mat4 View = Camera::GetViewMatrix(Scene, Scene->MainCam.Id);
            glm::mat4 Model = ET.GetTransformMatrix();

            for (auto &Mesh : EM.Meshes) {
                auto &Mat = Mesh.MeshMaterial;
                auto &Textures = Mat.Textures;

                Mat.Shader.Bind();
                Mat.Shader.SetMat4("view", View);
                Mat.Shader.SetMat4("projection", Projection);
                Mat.Shader.SetMat4("model", Model);

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
                    glBindTexture(GL_TEXTURE_2D, Textures[i].second.id);
                }
                glActiveTexture(GL_TEXTURE0);

                glBindVertexArray(Mesh.VAO);
                glDrawElements(GL_TRIANGLES, Mesh.Indicies.size(), GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
            }
        }
    }
}// namespace engine::systems
