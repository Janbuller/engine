#include <sol/sol.hpp>
#include <glm/glm.hpp>

namespace engine::lua::lualib {
    void LoadMathLib(sol::table &L);

    glm::quat Rotate_UD(glm::quat &Quat, float Angle, glm::vec3 &Axis);
    sol::table Rotate_TABLE(sol::this_state State, sol::table IQuat, float IAngle, sol::table IAxis);

    glm::vec3 RotatePoint_UD(glm::vec3 Point, glm::quat Quat);
    sol::table RotatePoint_TABLE(sol::this_state State, sol::table IPoint, sol::table IQuat);

    glm::quat InverseQuat_UD(glm::quat Quat);
    sol::table InverseQuat_TABLE(sol::this_state State, sol::table IQuat);
}// namespace engine::lualib
