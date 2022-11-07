#pragma once

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

namespace engine::components {
    struct Script {
        std::vector<std::string> ScriptPaths;

        Script() = default;
        Script(std::vector<std::string> ScriptPaths) : ScriptPaths{ScriptPaths} {}

        void AddScript(std::string Filename);
        void RemoveScript(std::string Filename);
    };
}// namespace engine::components
