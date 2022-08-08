#include "engine/lua/usertypes/model/LVertex.h"
#include "engine/model/Vertex.h"

namespace engine::lua::usertypes {
    void InitializeVertex(sol::state &L) {

        // Register the new usertype
        L.new_usertype<Vertex>(
                // Name
                "Vertex",

                sol::constructors<Vertex(),
                                  Vertex(glm::vec3, glm::vec3, glm::vec2, glm::vec3)>(),

                // Member variables
                "Position", &Vertex::Position,
                "Normal", &Vertex::Normal,
                "TexCoords", &Vertex::TexCoords,
                "Tangent", &Vertex::Tangent);
    }
}// namespace engine::lua::usertypes
