local Model = {}

function Model:new()
   local model = {}

   model.Meshes = {};
   model.Visible = true;

   setmetatable(model, self)
   self.__index = self
   return model
end

return Model
