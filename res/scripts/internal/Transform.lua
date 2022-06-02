local Vector3 = require("res/scripts/internal/Vector3")
local Quaternion = require("res/scripts/internal/Quaternion")

local Transform = {}

function Transform:new()
   local transform = {}

   transform.Position = Vector3:new(0, 0, 0);
   transform.Rotation = Quaternion:new(0, 0, 0, 1)
   transform.Scale = Vector3:new(0, 0, 0);
   
   setmetatable(transform, self)
   self.__index = self
   return transform
end

return Transform
