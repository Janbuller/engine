#include "engine/lua/usertypes/math/LVec3.h"
#include "glm/gtx/string_cast.hpp"
#include <glm/detail/type_vec3.hpp>
#include <glm/glm.hpp>

namespace engine::lua::usertypes {
    void InitializeVec3(sol::state &L) {

        // Create overloads for the different operators
        auto add_overloads = sol::overload(
                [](const glm::vec3 &v1, const glm::vec3 &v2) -> glm::vec3 { return v1 + v2; },
                [](const glm::vec3 &v1, float v) -> glm::vec3 { return v1 + v; },
                [](float v, const glm::vec3 &v1) -> glm::vec3 { return v + v1; });

        auto sub_overloads = sol::overload(
                [](const glm::vec3 &v1, const glm::vec3 &v2) -> glm::vec3 { return v1 - v2; },
                [](const glm::vec3 &v1, float v) -> glm::vec3 { return v1 - v; },
                [](float v, const glm::vec3 &v1) -> glm::vec3 { return v - v1; });

        auto mul_overloads = sol::overload(
                [](const glm::vec3 &v1, const glm::vec3 &v2) -> glm::vec3 { return v1 * v2; },
                [](const glm::vec3 &v1, float v) -> glm::vec3 { return v1 * v; },
                [](float v, const glm::vec3 &v1) -> glm::vec3 { return v * v1; });

        auto div_overloads = sol::overload(
                [](const glm::vec3 &v1, const glm::vec3 &v2) -> glm::vec3 { return v1 / v2; },
                [](const glm::vec3 &v1, float v) -> glm::vec3 { return v1 / v; },
                [](float v, const glm::vec3 &v1) -> glm::vec3 { return v / v1; });

        // Register the new usertype
        L.new_usertype<glm::vec3>(
                // Name
                "Vec3",

                sol::constructors<glm::vec3(),
                                  glm::vec3(float),
                                  glm::vec3(float, glm::vec2),
                                  glm::vec3(glm::vec2, float),
                                  glm::vec3(float, float, float),
                                  glm::vec3(glm::vec3)>(),

                // Case-insensitive xyz and rgb
                "x", &glm::vec3::x,
                "y", &glm::vec3::y,
                "z", &glm::vec3::z,

                "X", &glm::vec3::x,
                "Y", &glm::vec3::y,
                "Z", &glm::vec3::z,

                "r", &glm::vec3::x,
                "g", &glm::vec3::y,
                "b", &glm::vec3::z,

                "R", &glm::vec3::x,
                "G", &glm::vec3::y,
                "B", &glm::vec3::z,

                // Meta Functions
                sol::meta_function::to_string, [](glm::vec3 &v) { return glm::to_string(v); },
                sol::meta_function::equal_to, [](glm::vec3 &v1, glm::vec3 &v2) { return v1 == v2; },
                sol::meta_function::addition, add_overloads,
                sol::meta_function::subtraction, sub_overloads,
                sol::meta_function::multiplication, mul_overloads,
                sol::meta_function::division, div_overloads,
                sol::meta_function::unary_minus, [](glm::vec3 &v) { return -v; },

                // Other vector functions
                "Length", &glm::vec3::length,
                "Normalize", [](glm::vec3 &v) { return glm::normalize(v); },
                "Cross", [](glm::vec3 &v1, glm::vec3 &v2) { return glm::cross(v1, v2); },
                "Sum", [](glm::vec3 &v) { return v.x + v.y + v.z; });
    }
}// namespace engine::lua::usertypes
