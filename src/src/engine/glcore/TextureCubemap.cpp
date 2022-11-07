#include "engine/glcore/TextureCubemap.h"
#include "engine/Base.h"
#include "engine/core/Logger.h"
#include "engine/glcore/Texture.h"
#include "stb/stb_image.h"
#include <glad/glad.h>
#include <iostream>
#include <string>

namespace engine::glcore {
    TextureCubemap::TextureCubemap(GLuint Handle)
        : Texture{Handle} {
    }

    TextureCubemap TextureCubemap::FromTextureData(std::array<TextureData, 6> TD) {
        unsigned int TextureID;
        glGenTextures(1, &TextureID);

        for (int i = 0; i < 6; i++) {
            GLenum Format = GL_RG;
            if (TD[i].ComponentAmount == 1)
                Format = GL_RED;
            else if (TD[i].ComponentAmount == 3)
                Format = GL_RGB;
            else if (TD[i].ComponentAmount == 4)
                Format = GL_RGBA;

            glBindTexture(GL_TEXTURE_CUBE_MAP, TextureID);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, Format, TD[i].Width, TD[i].Height, 0, Format, GL_UNSIGNED_BYTE, TD[i].Data);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glGenerateMipmap(GL_TEXTURE_2D);

        return TextureCubemap{TextureID};
    }

}// namespace engine::glcore
