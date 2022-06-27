#pragma once

#include "engine/glcore/Texture.h"
#include <string>

namespace engine::glcore {
    class TextureData {
    public:
        unsigned char* Data;
        int Width, Height;
        int ComponentAmount;

        TextureData() = default;
        TextureData(unsigned char* Data, int Width, int Height, int ComponentAmount);
        void Free();

        static TextureData Load(std::string Path);
        static TextureData LoadTextureFromFile(const char *Path);
        static TextureData LoadTextureFromFile(const char *Path, const std::string &Directory);
    };
}
