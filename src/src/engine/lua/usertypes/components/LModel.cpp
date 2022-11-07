#include "engine/lua/usertypes/components/LModel.h"
#include "engine/ecs/components/Model.h"

namespace engine::lua::usertypes {
    void InitializeModel(sol::state &L) {

        // Register the new usertype
        L.new_usertype<components::Model>(
                // Name
                "Model",

                sol::constructors<components::Model()>(),

                // Member variables
                "LastLoadPath", &components::Model::LastLoadPath,
                "Visible", &components::Model::Visible,
                "Meshes",    &components::Model::Meshes);
    }
}// namespace engine::lua::usertypes
