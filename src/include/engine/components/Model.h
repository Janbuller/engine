#pragma once

#include "engine/Mesh.h"
#include "engine/Vertex.h"
#include <lua.hpp>
#include <LuaBridge/LuaBridge.h>
#include <vector>
#include "engine/Base.h"

namespace engine::components {
    struct Model {
        bool Visible = true;
        std::vector<Mesh> Meshes;

        Model() {
            Visible = true;
        }

        luabridge::LuaRef GetTable(lua_State *L) const {
	  auto L_ModelClass = luabridge::getGlobal(L, "Model");
	  auto L_MeshClass = luabridge::getGlobal(L, "Mesh");
	  auto L_VertexClass = luabridge::getGlobal(L, "Vertex");

	  auto L_Model = L_ModelClass["new"](L_ModelClass);

	  L_Model["Visible"] = Visible;

	  for(int MeshIdx = 0; MeshIdx < Meshes.size(); MeshIdx++) {
	    L_Model["Meshes"][MeshIdx+1] = L_MeshClass["new"](L_MeshClass);

	    for(int VertIdx = 0; VertIdx < Meshes[MeshIdx].Vertices.size(); VertIdx++) {
	      L_Model["Meshes"][MeshIdx+1]["Vertices"][VertIdx+1] = L_VertexClass["new"](L_VertexClass);
	      auto L_Vert = L_Model["Meshes"][MeshIdx+1]["Vertices"][VertIdx+1];

	      const auto& Vert = Meshes[MeshIdx].Vertices[VertIdx];

	      // L_Vert = Vert;

	      L_Vert["Position"]["x"] = Vert.Position.x;
	      L_Vert["Position"]["y"] = Vert.Position.y;
	      L_Vert["Position"]["z"] = Vert.Position.z;

	      L_Vert["Normal"]["x"] = Vert.Normal.x;
	      L_Vert["Normal"]["y"] = Vert.Normal.y;
	      L_Vert["Normal"]["z"] = Vert.Normal.z;

	      L_Vert["TexCoords"]["x"] = Vert.TexCoords.x;
	      L_Vert["TexCoords"]["y"] = Vert.TexCoords.y;
	    }

	    for(int IndIdx = 0; IndIdx < Meshes[MeshIdx].Indicies.size(); IndIdx++) {
	      L_Model["Meshes"][MeshIdx+1]["Indicies"][IndIdx+1] = Meshes[MeshIdx].Indicies[IndIdx];
	    }
	  }

	  return L_Model;
        };

        void SetTable(luabridge::LuaRef Component) {
            Visible = Component["Visible"];

	    std::vector<Mesh> NewMeshes{};
	    auto L_Meshes = Component["Meshes"];
	    for(int MeshIdx = 1; MeshIdx < L_Meshes.length()+1; MeshIdx++) {
	      auto L_Mesh = L_Meshes[MeshIdx];

	      std::vector<Vertex> Verts{};
	      std::vector<unsigned int> Indicies{};

	      for(int VertIdx = 1; VertIdx < L_Mesh["Vertices"].length()+1; VertIdx++) {
		auto L_Vert = L_Mesh["Vertices"][VertIdx];

		Verts.push_back({{
		      L_Vert["Position"]["x"],
		      L_Vert["Position"]["y"],
		      L_Vert["Position"]["z"]
		    },{
		      L_Vert["Normal"]["x"],
		      L_Vert["Normal"]["y"],
		      L_Vert["Normal"]["z"]
		    },{
		      L_Vert["TexCoords"]["x"],
		      L_Vert["TexCoords"]["y"],
		    }});
	      }
	      for(int IndIdx = 1; IndIdx < L_Mesh["Indicies"].length()+1; IndIdx++) {
		auto L_Ind = L_Mesh["Indicies"][IndIdx];

		Indicies.push_back(L_Ind);
	      }

	      if(MeshIdx-1 < Meshes.size()) {
		NewMeshes.push_back(Mesh{Verts, Indicies, Meshes[MeshIdx-1].MeshMaterial});
	      }
	      else {
		NewMeshes.push_back(Mesh{Verts, Indicies, Meshes[0].MeshMaterial});
	      }
	    }

	    Meshes.clear();
	    Meshes = NewMeshes;

	    // LOG_ENGINE_ERROR(Meshes.size());
        }

        std::string GetName() const {
            return "Model";
        }

        static Model Load(std::string Path);
    };
}// namespace engine::components
