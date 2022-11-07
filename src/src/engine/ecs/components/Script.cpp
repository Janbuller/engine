#include "engine/ecs/components/Script.h"
#include <algorithm>

namespace engine::components {
    void Script::AddScript(std::string Filename) {
        ScriptPaths.push_back(Filename);
    }
    void Script::RemoveScript(std::string Filename) {
        ScriptPaths.erase(std::find(ScriptPaths.begin(), ScriptPaths.end(), Filename));
    }
}// namespace engine::components
