#include "glcore/Texture.h"
#include "stb/stb_image.h"
#include <glad/glad.h>
#include <iostream>
#include <string>

namespace glcore {
    Texture::Texture(unsigned int id, std::string path, int width, int height)
        : id{id}, path{path}, width{width}, height{height} {
    }

    Texture Texture::LoadTextureFromFile(const char *path) {
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char *data =
                stbi_load(path, &width, &height, &nrComponents, 0);
        if (data) {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                         GL_UNSIGNED_BYTE, data);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                            GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glGenerateMipmap(GL_TEXTURE_2D);

            stbi_image_free(data);
        } else {
            std::cout << "Failed to load texture: " << path << std::endl;
            stbi_image_free(data);
        }

        return Texture{textureID, path, width, height};
    }

    Texture Texture::LoadTextureFromFile(const char *path,
                                         const std::string &directory) {
        std::string filename = std::string(path);
        filename = directory + '/' + filename;

        return LoadTextureFromFile(filename.c_str());
    }
}// namespace glcore
