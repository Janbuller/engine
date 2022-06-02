#include "engine/ModelLoader.h"
#include "engine/components/Model.h"

namespace engine::components {
  Model Model::Load(std::string Path) {
	return *ModelLoader::LoadModel(Path);
      }
}
