#include <sol/sol.hpp>

namespace engine::lualib {
    void LoadMathLib(sol::table &L);

    sol::table Rotate(sol::this_state State, sol::table IQuat, float IAngle, sol::table IAxis);
    sol::table RotatePoint(sol::this_state State, sol::table IPoint, sol::table IQuat);
    sol::table InverseQuat(sol::this_state State, sol::table IQuat);
}// namespace engine::lualib
