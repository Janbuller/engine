#pragma once

#include "glcore/Shader.h"
#include "glcore/Texture.h"
#include <string>
#include <utility>
#include <vector>

namespace engine {
  struct Material {
    glcore::Shader Shader;
    std::vector<std::pair<std::string, glcore::Texture>> Textures;

    Material() {}
    Material(glcore::Shader Shader, std::vector<std::pair<std::string, glcore::Texture>> Textures) : Shader(Shader), Textures(Textures) {}
  };
}
