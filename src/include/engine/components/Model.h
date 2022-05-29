#pragma once

#include "engine/Mesh.h"
#include <vector>
namespace engine::components {
  struct Model {
    std::vector<Mesh> Meshes;
  };
}
