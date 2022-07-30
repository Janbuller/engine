#pragma once

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

namespace engine::components {
    struct Script {
        std::vector<std::string> ScriptPaths;
    };
}// namespace engine::components
