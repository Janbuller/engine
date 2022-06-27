#pragma once

#include "engine/glcore/Shader.h"
#include "engine/glcore/Texture2D.h"
#include <string>
#include <utility>
#include <vector>

namespace engine {
  struct Material {
    glcore::Shader Shader;
    std::vector<std::pair<std::string, glcore::Texture2D>> Textures;

    Material() {}
    Material(glcore::Shader Shader, std::vector<std::pair<std::string, glcore::Texture2D>> Textures) : Shader(Shader), Textures(Textures) {}
  };
}
