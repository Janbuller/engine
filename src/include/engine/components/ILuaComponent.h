#pragma once

#include <lua.hpp>
#include <LuaBridge/LuaBridge.h>
#include <LuaBridge/detail/LuaRef.h>
namespace engine::components {
  class ILuaComponent {
    virtual luabridge::LuaRef GetTable(lua_State* L) = 0;
    virtual void SetTable(luabridge::LuaRef Component) = 0;
    virtual std::string GetName() = 0;
  };
}
