#include "engine/lua/usertypes/model/LMesh.h"
#include "engine/model/Mesh.h"

namespace engine::lua::usertypes {
    void InitializeMesh(sol::state &L) {

        // Register the new usertype
        L.new_usertype<Mesh>(
                // Name
                "Mesh",

                sol::constructors<Mesh(),
                                  Mesh(std::vector<Vertex>, std::vector<unsigned int>, Material)>(),

                // Member variables
                "Vertices", &Mesh::Vertices,
                "Indices", &Mesh::Indices,
                "Material", &Mesh::MeshMaterial,

                "SetupBuffers", &Mesh::SetupBuffers);
    }
}// namespace engine::lua::usertypes
