#include "engine/glcore/TextureData.h"
#include "engine/Base.h"
#include "stb/stb_image.h"

namespace engine::glcore {
    TextureData::TextureData(unsigned char *Data, int Width, int Height, int ComponentAmount)
        : Data(Data), Width(Width), Height(Height), ComponentAmount(ComponentAmount) {
    }

    void TextureData::Free() {
        stbi_image_free(Data);
    }

    TextureData TextureData::Load(std::string Path) {
        return LoadTextureFromFile(Path.c_str());
    }

    TextureData TextureData::LoadTextureFromFile(const char *Path) {
        int Width, Height, ComponentAmount;
        unsigned char *Data = stbi_load(Path, &Width, &Height, &ComponentAmount, 0);
        if (!Data) {
            LOG_ENGINE_ERROR("Failed to load texturedata: {0}", Path);
        }

        LOG_ENGINE_CRITICAL("{0}", ComponentAmount);
        LOG_ENGINE_ERROR("{0}", Data[Width * Height * ComponentAmount - 1]);
        LOG_ENGINE_ERROR("{0}", Width * Height * ComponentAmount - 1);
        return TextureData{Data, Width, Height, ComponentAmount};
    }

    TextureData TextureData::LoadTextureFromFile(const char *Path,
                                                 const std::string &Directory) {
        std::string Filename = std::string(Path);
        Filename = Directory + '/' + Filename;

        return LoadTextureFromFile(Filename.c_str());
    }
}// namespace engine::glcore
