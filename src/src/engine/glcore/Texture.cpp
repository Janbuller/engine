#include "engine/glcore/Texture.h"
#include "engine/core/Logger.h"

namespace engine::glcore {
    Texture::Texture(GLuint Handle, int Width, int Height)
        : Handle{Handle}, Width{Width}, Height{Height} {
    }
}
