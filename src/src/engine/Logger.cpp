#include "engine/Logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <memory>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>

namespace engine {
  sptr<spdlog::logger> Logger::EngineLogger;
  sptr<spdlog::logger> Logger::GameLogger;

  void Logger::Init() {
    EngineLogger = spdlog::stdout_color_mt("ENGINE");
    GameLogger   = spdlog::stdout_color_mt("GAME");

    EngineLogger->set_level(spdlog::level::trace);
    GameLogger->set_level(spdlog::level::trace);

    EngineLogger->set_pattern("[%H:%M:%S] [%n] [%^%l%$] %v");
    GameLogger->set_pattern("[%H:%M:%S] [%n]   [%^%l%$] %v");
  }
}
