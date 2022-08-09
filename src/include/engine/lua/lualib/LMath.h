#include <sol/sol.hpp>
#include <glm/glm.hpp>

namespace engine::lua::lualib {
    void LoadMathLib(sol::table &L);

    glm::quat Rotate(glm::quat &Quat, float Angle, glm::vec3 &Axis);
    sol::table Rotate(sol::this_state State, sol::table IQuat, float IAngle, sol::table IAxis);

    glm::vec3 RotatePoint(glm::vec3 Point, glm::quat Quat);
    sol::table RotatePoint(sol::this_state State, sol::table IPoint, sol::table IQuat);

    glm::quat InverseQuat(glm::quat Quat);
    sol::table InverseQuat(sol::this_state State, sol::table IQuat);
}// namespace engine::lualib
