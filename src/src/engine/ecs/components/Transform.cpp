#include "engine/ecs/components/Transform.h"
#include "engine/ecs/components/Model.h"
#include "engine/ModelLoader.h"

namespace engine::components {

    glm::mat4 Transform::GetTransformMatrix() {
        glm::mat4 TransMat = glm::mat4{1.0};

        TransMat = glm::translate(TransMat, Position);
        TransMat = glm::scale(TransMat, Scale);
        TransMat *= glm::mat4_cast(Rotation);

	return TransMat;
    }

    luabridge::LuaRef Transform::GetTable(lua_State *L) const {
        lua_getglobal(L, "Transform");
        auto L_TransformClass = luabridge::LuaRef::fromStack(L, -1);
        lua_pop(L, -1);

        auto L_Transform = L_TransformClass["new"](L_TransformClass);
        L_Transform["Position"]["x"] = Position.x;
        L_Transform["Position"]["y"] = Position.y;
        L_Transform["Position"]["z"] = Position.z;

        L_Transform["Rotation"]["x"] = Rotation.x;
        L_Transform["Rotation"]["y"] = Rotation.y;
        L_Transform["Rotation"]["z"] = Rotation.z;
        L_Transform["Rotation"]["w"] = Rotation.w;

        L_Transform["Scale"]["x"] = Scale.x;
        L_Transform["Scale"]["y"] = Scale.y;
        L_Transform["Scale"]["z"] = Scale.z;

        return L_Transform;
    }

    void Transform::SetTable(luabridge::LuaRef Component) {
        Position.x = Component["Position"]["x"];
        Position.y = Component["Position"]["y"];
        Position.z = Component["Position"]["z"];

        Rotation.x = Component["Rotation"]["x"];
        Rotation.y = Component["Rotation"]["y"];
        Rotation.z = Component["Rotation"]["z"];
        Rotation.w = Component["Rotation"]["w"];

        Scale.x = Component["Scale"]["x"];
        Scale.y = Component["Scale"]["y"];
        Scale.z = Component["Scale"]["z"];
    }
}
