#include "engine/lua/usertypes/math/LVec2.h"
#include "glm/gtx/string_cast.hpp"
#include <fmt/format.h>
#include <glm/detail/type_vec3.hpp>
#include <glm/glm.hpp>

namespace engine::lua::usertypes {
    void InitializeVec2(sol::state &L) {

        // Create overloads for the different operators
        auto add_overloads = sol::overload(
                [](const glm::vec2 &v1, const glm::vec2 &v2) -> glm::vec2 { return v1 + v2; },
                [](const glm::vec2 &v1, float v) -> glm::vec2 { return v1 + v; },
                [](float v, const glm::vec2 &v1) -> glm::vec2 { return v + v1; });

        auto sub_overloads = sol::overload(
                [](const glm::vec2 &v1, const glm::vec2 &v2) -> glm::vec2 { return v1 - v2; },
                [](const glm::vec2 &v1, float v) -> glm::vec2 { return v1 - v; },
                [](float v, const glm::vec2 &v1) -> glm::vec2 { return v - v1; });

        auto mul_overloads = sol::overload(
                [](const glm::vec2 &v1, const glm::vec2 &v2) -> glm::vec2 { return v1 * v2; },
                [](const glm::vec2 &v1, float v) -> glm::vec2 { return v1 * v; },
                [](float v, const glm::vec2 &v1) -> glm::vec2 { return v * v1; });

        auto div_overloads = sol::overload(
                [](const glm::vec2 &v1, const glm::vec2 &v2) -> glm::vec2 { return v1 / v2; },
                [](const glm::vec2 &v1, float v) -> glm::vec2 { return v1 / v; },
                [](float v, const glm::vec2 &v1) -> glm::vec2 { return v / v1; });

        // Register the new usertype
        L.new_usertype<glm::vec2>(
                // Name
                "Vec2",

                sol::constructors<glm::vec2(),
                                  glm::vec2(float),
                                  glm::vec2(float, float),
                                  glm::vec2(glm::vec2)>(),

                // Case-insensitive xy
                "x", &glm::vec2::x,
                "y", &glm::vec2::y,

                "X", &glm::vec2::x,
                "Y", &glm::vec2::y,

                // Meta Functions
                sol::meta_function::to_string, [](glm::vec2 &v) { return fmt::format("Vec2.new({0}, {1})", v.x, v.y); },
                sol::meta_function::equal_to, [](glm::vec2 &v1, glm::vec2 &v2) { return v1 == v2; },
                sol::meta_function::addition, add_overloads,
                sol::meta_function::subtraction, sub_overloads,
                sol::meta_function::multiplication, mul_overloads,
                sol::meta_function::division, div_overloads,
                sol::meta_function::unary_minus, [](glm::vec2 &v) { return -v; },

                // Other vector functions
                "Length", &glm::vec2::length,
                "Normalize", [](glm::vec2 &v) { return glm::normalize(v); },
                "Sum", [](glm::vec2 &v) { return v.x + v.y;});
    }
}// namespace engine::lua::usertypes
