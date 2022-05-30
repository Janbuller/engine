#pragma once

#include "engine/Base.h"
#include "engine/Scene.h"
#include "engine/systems/ISystem.h"
#include <lua.hpp>

namespace engine::systems {
  class LuaScriptRunner : public ISystem {
  private:
    lua_State* L;

  public: 
    void Init(sptr<Scene> Scene) override;
    void Update(sptr<Scene> Scene, double DeltaTime) override;
    void Exit(sptr<Scene> Scene) override;

  private:
    bool CheckLua(lua_State *L, int r);
    std::string LoadAndReplaceFile(std::string filename);

    void SetComponentsInLua(sptr<Scene> Scene, const Entity& E);
    void SetComponentsInEngine(sptr<Scene> Scene, const Entity& E);
  };
}
