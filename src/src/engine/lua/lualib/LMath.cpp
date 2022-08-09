#include "engine/lua/lualib/LMath.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <sol/sol.hpp>

namespace engine::lua::lualib {
    void LoadMathLib(sol::table &L) {
        auto Math      = L["Math"].get_or_create<sol::table>();
        Math["Rotate"] = sol::overload(
                sol::resolve<glm::quat(glm::quat &, float, glm::vec3 &)>(Rotate),
                sol::resolve<sol::table(sol::this_state, sol::table, float, sol::table)>(Rotate));
        Math["RotatePoint"] = sol::overload(
                sol::overload<glm::vec3(glm::vec3, glm::quat)>(RotatePoint),
                sol::overload<sol::table(sol::this_state, sol::table, sol::table)>(RotatePoint));
        Math["InverseQuat"] = sol::overload(
                sol::overload<glm::quat(glm::quat)>(InverseQuat),
                sol::overload<sol::table(sol::this_state, sol::table)>(InverseQuat));
    }

    glm::quat Rotate(glm::quat &Quat, float Angle, glm::vec3 &Axis) {
        return glm::rotate(Quat, Angle, Axis);
    }

    sol::table Rotate(sol::this_state State, sol::table IQuat, float IAngle, sol::table IAxis) {
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

    glm::vec3 RotatePoint(glm::vec3 Point, glm::quat Quat) {
        return glm::rotate(Quat, Point);
    }

    sol::table RotatePoint(sol::this_state State, sol::table IPoint, sol::table IQuat) {
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

    glm::quat InverseQuat(glm::quat Quat) {
        return glm::inverse(Quat);
    }

    sol::table InverseQuat(sol::this_state State, sol::table IQuat) {
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
}// namespace engine::lua::lualib
