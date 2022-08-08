#include "engine/lua/lualib/LMath.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <sol/sol.hpp>

namespace engine::lua::lualib {
    void LoadMathLib(sol::table &L) {
        auto Math           = L["Math"].get_or_create<sol::table>();
        Math["Rotate"]      = sol::overload(Rotate_UD, Rotate_TABLE);
        Math["RotatePoint"] = sol::overload(RotatePoint_UD, RotatePoint_TABLE);
        Math["InverseQuat"] = sol::overload(InverseQuat_UD, InverseQuat_TABLE);
    }

    glm::quat Rotate_UD(glm::quat &Quat, float Angle, glm::vec3 &Axis) {
        return glm::rotate(Quat, Angle, Axis);
    }

    sol::table Rotate_TABLE(sol::this_state State, sol::table IQuat, float IAngle, sol::table IAxis) {
        sol::state_view StateView = State;

        glm::quat StartQuat{IQuat["w"], IQuat["x"], IQuat["y"], IQuat["z"]};
        const auto &Angle = IAngle;
        glm::vec3 Axis{IAxis["x"], IAxis["y"], IAxis["z"]};

        auto EndQuat = glm::rotate(StartQuat, Angle, Axis);

        auto ReturnQuat = StateView.create_table();
        ReturnQuat["w"] = EndQuat.w;
        ReturnQuat["x"] = EndQuat.x;
        ReturnQuat["y"] = EndQuat.y;
        ReturnQuat["z"] = EndQuat.z;

        return ReturnQuat;
    }

    glm::vec3 RotatePoint_UD(glm::vec3 Point, glm::quat Quat) {
        return glm::rotate(Quat, Point);
    }

    sol::table RotatePoint_TABLE(sol::this_state State, sol::table IPoint, sol::table IQuat) {
        sol::state_view StateView = State;

        glm::quat Quat{IQuat["w"], IQuat["x"], IQuat["y"], IQuat["z"]};
        glm::vec3 Point{IPoint["x"], IPoint["y"], IPoint["z"]};

        Point = glm::rotate(Quat, Point);

        auto ReturnPoint = StateView.create_table();
        ReturnPoint["x"] = Point.x;
        ReturnPoint["y"] = Point.y;
        ReturnPoint["z"] = Point.z;

        return ReturnPoint;
    }

    glm::quat InverseQuat_UD(glm::quat Quat) {
        return glm::inverse(Quat);
    }

    sol::table InverseQuat_TABLE(sol::this_state State, sol::table IQuat) {
        sol::state_view StateView = State;

        glm::quat Quat{IQuat["w"], IQuat["x"], IQuat["y"], IQuat["z"]};

        auto Inverse = glm::inverse(Quat);

        auto ReturnQuat = StateView.create_table();
        ReturnQuat["w"] = Inverse.w;
        ReturnQuat["x"] = Inverse.x;
        ReturnQuat["y"] = Inverse.y;
        ReturnQuat["z"] = Inverse.z;

        return ReturnQuat;
    }
}// namespace engine::lualib
