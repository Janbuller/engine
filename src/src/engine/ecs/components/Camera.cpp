#include "engine/ecs/components/Camera.h"

namespace engine::components {
    glm::mat4 Camera::GetProjectionMatrix(sptr<Scene> Scene, EntityID E, int Width, int Height) {
        auto &EC = Scene->GetComponent<Camera>(E);

        auto Aspect = (float) Width / (float) Height;
        /* Aspect *= EC.FOV; */

        if (EC.Projection == ProjectionType::PERSPECTIVE)
            return glm::perspective(glm::radians(EC.FOV), Aspect, EC.Near, EC.Far);
        else if (EC.Projection == ProjectionType::ORTHOGRAPHIC)
            return glm::ortho(-Aspect, Aspect, -1.0f, 1.0f, EC.Near, EC.Far);
        /* return glm::ortho(0.0f, (float) Width * EC.FOV, 0.0f, (float) Height * EC.FOV, EC.Near, EC.Far); */

        LOG_ENGINE_ERROR("Failed to get projection matrix from Entity \"{0}\" with ProjectionType \"{1}\".", E, (int) EC.Projection);
        throw std::runtime_error("Failed to get projection matrix.");
    }

    glm::mat4 Camera::GetViewMatrix(sptr<Scene> Scene, EntityID E) {
        auto &ET = Scene->GetComponent<Transform>(E);
        auto &EC = Scene->GetComponent<Camera>(E);

        /* LOG_ENGINE_CRITICAL(glm::to_string(ET.Position)); */
        glm::vec3 Front = glm::rotate(glm::inverse(ET.Rotation), glm::vec3(0.0, 0.0, -1.0));
        glm::vec3 Up    = glm::rotate(glm::inverse(ET.Rotation), glm::vec3(0.0, 1.0, 0.0));
        return glm::lookAt(ET.Position, ET.Position + Front, Up);
    }
}// namespace engine::components
