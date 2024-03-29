#pragma once

#include <memory>

namespace engine {
    template<typename T>
    using uptr = std::unique_ptr<T>;

    template<typename T>
    using sptr = std::shared_ptr<T>;
}// namespace engine

#include "engine/core/Logger.h"
