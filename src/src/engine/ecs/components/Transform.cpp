#include "engine/ecs/components/Transform.h"

namespace engine::components {
    glm::mat4 Transform::GetTransformMatrix() const {
        glm::mat4 TransMat = glm::mat4{1.0};

        TransMat = glm::translate(TransMat, Position);
        TransMat = glm::scale(TransMat, Scale);
        TransMat *= glm::mat4_cast(Rotation);

        return TransMat;
    }
}// namespace engine::components
