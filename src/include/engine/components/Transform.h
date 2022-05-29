#pragma once

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace engine::components {
  struct Transform {
    glm::vec3 Position = glm::vec3{0};
    glm::quat Rotation = glm::quat{0, 0, 0, 1};
    glm::vec3 Scale    = glm::vec3{1};
  };
}
