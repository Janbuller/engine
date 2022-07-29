#include "engine/glcore/Texture2D.h"
#include "engine/Base.h"
#include "engine/core/Logger.h"
#include "engine/glcore/Texture.h"
#include "stb/stb_image.h"
#include <glad/glad.h>
#include <iostream>
#include <string>

namespace engine::glcore {
    Texture2D::Texture2D(GLuint Handle, int Width, int Height)
        : Texture{Handle}, Width{Width}, Height{Height} {
    }

    Texture2D Texture2D::FromTextureData(TextureData TD) {
        unsigned int TextureID;
        glGenTextures(1, &TextureID);

        GLenum Format;
        if (TD.ComponentAmount == 1)
            Format = GL_RED;
        else if (TD.ComponentAmount == 3)
            Format = GL_RGB;
        else if (TD.ComponentAmount == 4)
            Format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, TextureID);
        glTexImage2D(GL_TEXTURE_2D, 0,
                     Format, TD.Width, TD.Height, 0,
                     Format, GL_UNSIGNED_BYTE, TD.Data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        glGenerateMipmap(GL_TEXTURE_2D);

        return Texture2D{TextureID, TD.Width, TD.Height};
    }

}// namespace engine::glcore
