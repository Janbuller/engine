local Mesh = {}

function Mesh:new()
   local mesh = {}

   mesh.Vertices = {};
   mesh.Indices = {};

   setmetatable(mesh, self)
   self.__index = self
   return mesh
end

return Mesh
