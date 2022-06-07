#pragma once

#include "engine/ecs/components/Transform.h"
#include "engine/ecs/systems/ISystem.h"
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>

namespace engine::components {
    struct Camera {
        enum class ProjectionType {
            PERSPECTIVE,
            ORTHOGRAPHIC
        };

        float FOV = 45.0f;

        float Near = 0.3f;
        float Far = 5000.0f;

        ProjectionType Projection = ProjectionType::PERSPECTIVE;

        static glm::mat4 GetProjectionMatrix(sptr<Scene> Scene, EntityID E, int Width, int Height) {
            auto& EC = Scene->GetComponent<Camera>(E);

            if(EC.Projection == ProjectionType::PERSPECTIVE)
                return glm::perspective(glm::radians(EC.FOV), (float) Width / (float) Height, EC.Near, EC.Far);
            else if(EC.Projection == ProjectionType::ORTHOGRAPHIC)
                return glm::ortho(0.0f, (float)Width * EC.FOV, 0.0f, (float)Height * EC.FOV, EC.Near, EC.Far);

            LOG_ENGINE_ERROR("Failed to get projection matrix from Entity \"{0}\" with ProjectionType \"{1}\".", E, (int)EC.Projection);
            throw std::runtime_error("Failed to get projection matrix.");
        }

        static glm::mat4 GetViewMatrix(sptr<Scene> Scene, EntityID E) {
            auto& ET = Scene->GetComponent<Transform>(E);
            auto& EC = Scene->GetComponent<Camera>(E);

            glm::vec3 Front = glm::rotate(glm::inverse(ET.Rotation), glm::vec3(0.0, 0.0, -1.0));
            glm::vec3 Up = glm::rotate(glm::inverse(ET.Rotation), glm::vec3(0.0, 1.0, 0.0));
            return glm::lookAt(ET.Position, ET.Position + Front, Up);
        }
    };

}// namespace engine::components
