#pragma once

#include "engine/Base.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <memory>
#include <spdlog/spdlog.h>

namespace engine {
    class Logger {
    public:
        static void Init();

        static sptr<spdlog::logger> EngineLogger;
        static sptr<spdlog::logger> GameLogger;
    };
}// namespace engine

#ifndef RELEASE

#define LOG_ENGINE_TRACE(...)    ::engine::Logger::EngineLogger->trace(__VA_ARGS__)
#define LOG_ENGINE_INFO(...)     ::engine::Logger::EngineLogger->info(__VA_ARGS__)
#define LOG_ENGINE_WARN(...)     ::engine::Logger::EngineLogger->warn(__VA_ARGS__)
#define LOG_ENGINE_ERROR(...)    ::engine::Logger::EngineLogger->error(__VA_ARGS__)
#define LOG_ENGINE_CRITICAL(...) ::engine::Logger::EngineLogger->critical(__VA_ARGS__)

#define LOG_TRACE(...)    ::engine::Logger::GameLogger->trace(__VA_ARGS__)
#define LOG_INFO(...)     ::engine::Logger::GameLogger->info(__VA_ARGS__)
#define LOG_WARN(...)     ::engine::Logger::GameLogger->warn(__VA_ARGS__)
#define LOG_ERROR(...)    ::engine::Logger::GameLogger->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::engine::Logger::GameLogger->critical(__VA_ARGS__)

#else

#define LOG_ENGINE_TRACE(...)    
#define LOG_ENGINE_INFO(...)     
#define LOG_ENGINE_WARN(...)     
#define LOG_ENGINE_ERROR(...)    
#define LOG_ENGINE_CRITICAL(...) 

#define LOG_TRACE(...)    
#define LOG_INFO(...)     
#define LOG_WARN(...)     
#define LOG_ERROR(...)    
#define LOG_CRITICAL(...) 

#endif



