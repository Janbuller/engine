#pragma once

#include "engine/Base.h"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <sol/sol.hpp>

namespace engine::components {
    struct Transform {
        glm::vec3 Position = glm::vec3{0};
        glm::quat Rotation = glm::quat{1, 0, 0, 0};
        glm::vec3 Scale    = glm::vec3{1};

        Transform() = default;

        glm::mat4 GetTransformMatrix() const;

        sol::table GetTable(sol::state &L) const;
        void SetTable(sol::table Component);

        std::string GetName() const { return "Transform"; }
    };
}// namespace engine::components
