local Vector3 = require("res/Internal/Scripts/Vector3")
local Vector2 = require("res/Internal/Scripts/Vector2")

local Vertex = {}

function Vertex:new()
   local vertex = {}

   vertex.Position = Vector3:new(0, 0, 0);
   vertex.Normal = Vector3:new(0, 0, 0);
   vertex.TexCoords = Vector2:new(0, 0);
   
   setmetatable(vertex, self)
   self.__index = self
   return vertex
end

return Vertex
