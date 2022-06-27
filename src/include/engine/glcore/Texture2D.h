#pragma once

#include "engine/glcore/Texture.h"
#include "engine/glcore/TextureData.h"
#include <glad/glad.h>
#include <string>

namespace engine::glcore {
    struct Texture2D : public Texture {
        Texture2D() = default;
        Texture2D(GLuint Handle, int Width, int Height);
        static Texture2D FromTextureData(TextureData TD);

        /*
        static Texture2D Load(std::string Path);
        static Texture2D LoadTextureFromFile(const char *Path);
        static Texture2D LoadTextureFromFile(const char *Path, const std::string &Directory);
        */
    };
}// namespace engine::glcore
