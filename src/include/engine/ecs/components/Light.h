#pragma once

#include <glm/glm.hpp>

namespace engine::components {
    struct Light {
        glm::vec3 Color;
        float Intensity;

        float Constant;
        float Linear;
        float Quadratic;

        enum LightType {
            PointLight,
            DirectionalLight
        };
        LightType Type;
    };
}// namespace engine::components
