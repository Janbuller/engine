#include "engine/lua/usertypes/components/LScript.h"
#include "engine/ecs/components/Script.h"

namespace engine::lua::usertypes {
    void InitializeScript(sol::state &L) {

        // Register the new usertype
        L.new_usertype<components::Script>(
                // Name
                "Script",

                sol::constructors<components::Script(),
                                  components::Script(std::vector<std::string>)>(),

                // Member variables
                "ScriptPaths", &components::Script::ScriptPaths);
    }
}// namespace engine::lua::usertypes
