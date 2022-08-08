#pragma once

#include <glm/glm.hpp>

namespace engine::components {
    struct Light {
        glm::vec3 Color;
        float Intensity;

        float Constant;
        float Linear;
        float Quadratic;

        enum class LightType {
            PointLight,
            DirectionalLight
        };
        LightType Type;

        Light() = default;
        Light(glm::vec3 Color,
              float Intensity,
              float Constant,
              float Linear,
              float Quadratic,
              LightType Type) : Color{Color},
                                Intensity{Intensity},
                                Constant{Constant},
                                Linear{Linear},
                                Quadratic{Quadratic},
                                Type{Type} {}
    };
}// namespace engine::components
