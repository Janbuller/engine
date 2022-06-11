#pragma once

#include <string>

namespace engine::glcore {
    struct Texture {
        unsigned int id;
        std::string path;

        int width, height;

        Texture() = default;
        Texture(unsigned int id, std::string path, int width, int height);

        static Texture Load(std::string path);
        static Texture LoadTextureFromFile(const char *path);
        static Texture LoadTextureFromFile(const char *path, const std::string &directory);
    };
}// namespace engine::glcore
