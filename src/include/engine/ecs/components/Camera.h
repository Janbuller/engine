#pragma once

#include "engine/ecs/Scene.h"
#include "engine/ecs/components/Transform.h"
#include "engine/glcore/TextureCubemap.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <optional>

namespace engine::components {
    struct Camera {
        enum class ProjectionType {
            PERSPECTIVE,
            ORTHOGRAPHIC
        };

        float FOV = 45.0f;

        float Near = 0.3f;
        float Far  = 5000.0f;

        ProjectionType Projection = ProjectionType::PERSPECTIVE;

        std::optional<glm::vec4> BackgroundColor{};
        std::optional<glcore::TextureCubemap> Skybox{};

        static glm::mat4 GetProjectionMatrix(sptr<Scene> Scene, EntityID E, int Width, int Height);
        static glm::mat4 GetViewMatrix(sptr<Scene> Scene, EntityID E);

        Camera() = default;
        Camera(float FOV,
               float Near,
               float Far,
               ProjectionType Projection) : FOV{FOV},
                                            Near{Near},
                                            Far{Far},
                                            Projection{Projection} {}
    };

}// namespace engine::components
