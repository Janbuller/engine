#include "engine/lua/usertypes/components/LCamera.h"
#include "engine/ecs/components/Camera.h"

namespace engine::lua::usertypes {
    void InitializeCamera(sol::state &L) {

        // Register the new usertype
        L.new_usertype<components::Camera>(
                // Name
                "Camera",

                sol::constructors<components::Camera(float, float, float, components::Camera::ProjectionType)>(),

                // Member variables
                "FOV", &components::Camera::FOV,

                "Near", &components::Camera::Near,
                "Far", &components::Camera::Far,

                "Projection", &components::Camera::Projection,

                "BackgroundColor", &components::Camera::BackgroundColor,
                "Skybox", &components::Camera::Skybox);

        L.new_enum(
                // Name
                "Camera.ProjectionType",

                // Values
                "PointLight", components::Camera::ProjectionType::PERSPECTIVE,
                "PointLight", components::Camera::ProjectionType::ORTHOGRAPHIC);
    }
}// namespace engine::lua::usertypes
