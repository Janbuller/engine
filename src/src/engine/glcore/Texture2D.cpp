#include "engine/core/Logger.h"
#include "engine/glcore/Texture.h"
#include "engine/glcore/Texture2D.h"
#include "engine/Base.h"
#include "stb/stb_image.h"
#include <glad/glad.h>
#include <iostream>
#include <string>

namespace engine::glcore {
    Texture2D::Texture2D(GLuint Handle, int Width, int Height)
        : Texture{Handle, Width, Height} {
    }

    Texture2D Texture2D::FromTextureData(TextureData TD) {
        unsigned int TextureID;
        glGenTextures(1, &TextureID);

        LOG_ENGINE_WARN("{0}", TextureID);

        GLenum Format;
        if (TD.ComponentAmount == 1)
            Format = GL_RED;
        else if (TD.ComponentAmount == 3)
            Format = GL_RGB;
        else if (TD.ComponentAmount == 4)
            Format = GL_RGBA;

        LOG_ENGINE_CRITICAL("{0} : {1}", Format, GL_RGB);

        glBindTexture(GL_TEXTURE_2D, TextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, Format, TD.Width, TD.Height, 0, Format,
                     GL_UNSIGNED_BYTE, TD.Data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        glGenerateMipmap(GL_TEXTURE_2D);

        return Texture2D{TextureID, TD.Width, TD.Height};
    }

    /*
    Texture2D Texture2D::Load(std::string Path) {
        return LoadTextureFromFile(Path.c_str());
    }

    Texture2D Texture2D::LoadTextureFromFile(const char *Path) {
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int Width, Height, ComponentAmount;
        unsigned char *Data =
                stbi_load(Path, &Width, &Height, &ComponentAmount, 0);
        if (Data) {
            GLenum format;
            if (ComponentAmount == 1)
                format = GL_RED;
            else if (ComponentAmount == 3)
                format = GL_RGB;
            else if (ComponentAmount == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, Width, Height, 0, format,
                         GL_UNSIGNED_BYTE, Data);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                            GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

            glGenerateMipmap(GL_TEXTURE_2D);

            stbi_image_free(Data);
        } else {
            LOG_ENGINE_ERROR("Failed to load texture: {0}", Path);
            stbi_image_free(Data);
        }

        return Texture2D{textureID, Width, Height};
    }

    Texture2D Texture2D::LoadTextureFromFile(const char *Path,
                                             const std::string &Directory) {
        std::string Filename = std::string(Path);
        Filename = Directory + '/' + Filename;

        return LoadTextureFromFile(Filename.c_str());
    }
    */
}// namespace engine::glcore
