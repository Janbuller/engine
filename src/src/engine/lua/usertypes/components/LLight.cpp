#include "engine/lua/usertypes/components/LLight.h"
#include "engine/ecs/components/Light.h"

namespace engine::lua::usertypes {
    void InitializeLight(sol::state &L) {

        // Register the new usertype
        L.new_usertype<components::Light>(
                // Name
                "Light",

                sol::constructors<components::Light(glm::vec3, float, float, float, float, components::Light::LightType)>(),

                // Member variables
                "Color",     &components::Light::Color,
                "Intensity", &components::Light::Intensity,

                "Constant",  &components::Light::Constant,
                "Linear",    &components::Light::Linear,
                "Quadratic", &components::Light::Quadratic,

                "Type",      &components::Light::Type);

        L.new_enum(
                // Name
                "LightType",

                // Values
                "PointLight",       components::Light::LightType::PointLight,
                "DirectionalLight", components::Light::LightType::DirectionalLight);

    }
}// namespace engine::lua::usertypes
