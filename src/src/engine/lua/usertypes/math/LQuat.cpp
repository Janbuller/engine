#include "engine/lua/usertypes/math/LQuat.h"
#include "glm/gtx/string_cast.hpp"
#include <fmt/format.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace engine::lua::usertypes {
    void InitializeQuat(sol::state &L) {

        // Create overloads for the multiplication operator
        auto mul_overloads = sol::overload(
                [](const glm::quat &v1, const glm::quat &v2) -> glm::quat { return v1 * v2; },
                [](const glm::quat &v1, const glm::vec3 &v2) -> glm::vec3 { return v1 * v2; },
                [](const glm::vec3 &v1, const glm::quat &v2) -> glm::vec3 { return v1 * v2; },
                [](const glm::quat &v1, const float &v) -> glm::quat { return v * v1; },
                [](const float v, const glm::quat &v1) -> glm::quat { return v * v1; });

        // Register the new usertype
        L.new_usertype<glm::quat>(
                // Name
                "Quat",

                sol::constructors<glm::quat(),
                                  glm::quat(float, float, float, float),
                                  glm::quat(glm::quat)>(),

                // Case-insensitive xyzw
                "w", &glm::quat::w,
                "x", &glm::quat::x,
                "y", &glm::quat::y,
                "z", &glm::quat::z,

                "W", &glm::quat::w,
                "X", &glm::quat::x,
                "Y", &glm::quat::y,
                "Z", &glm::quat::z,

                // Meta Functions
                sol::meta_function::to_string, [](glm::quat &v) { return fmt::format("Quat.new({0}, {1}, {2}, {3})", v.w, v.x, v.y, v.z); },
                sol::meta_function::equal_to, [](glm::quat &v1, glm::quat &v2) { return v1 == v2; },
                /* sol::meta_function::multiplication, mul_overloads, */
                sol::meta_function::unary_minus, [](glm::quat &v) { return -v; },

                // Other quaternion functions
                "Length", &glm::quat::length,
                "Rotate", [](glm::quat &Self, float Angle, glm::vec3 &Axis) { Self = glm::rotate(Self, Angle, Axis); },
                "RotatePoint", [](glm::quat &Quat, glm::vec3 &Point) -> glm::vec3 { return glm::rotate(Quat, Point); },
                "Inverse", [](glm::quat &v) -> glm::quat { return glm::inverse(v); });
    }
}// namespace engine::lua::usertypes
