#pragma once

#include <memory>

namespace engine {
  template<typename T>
  using uptr = std::unique_ptr<T>;

  template<typename T>
  using sptr = std::shared_ptr<T>;
}

#include "engine/Logger.h"
