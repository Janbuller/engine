#pragma once

#include <string>
namespace engine {
    struct TextureTypeInfo {
        std::string ShaderName;

        TextureTypeInfo() = default;
        TextureTypeInfo(std::string ShaderName) : ShaderName{ShaderName}  {}
    };
}// namespace engine
