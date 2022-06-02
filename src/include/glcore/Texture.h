#pragma once

#include <string>

namespace glcore {
    struct Texture {
      unsigned int id;
      std::string path;

      int width, height;

      Texture() {}
      Texture(unsigned int id, std::string path, int width, int height);

      static Texture Load(std::string path);
      static Texture LoadTextureFromFile(const char* path);
      static Texture LoadTextureFromFile(const char* path, const std::string &directory);
    };
}

