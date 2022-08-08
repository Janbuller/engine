#include "engine/lua/usertypes/components/LTransform.h"
#include "engine/ecs/components/Transform.h"

namespace engine::lua::usertypes {
    void InitializeTransform(sol::state &L) {

        // Register the new usertype
        L.new_usertype<components::Transform>(
                // Name
                "Transform",

                sol::constructors<components::Transform()>(),

                // Member variables
                "Position", &components::Transform::Position,
                "Rotation", &components::Transform::Rotation,
                "Scale",    &components::Transform::Scale);
    }
}// namespace engine::lua::usertypes
