local MT = {
   __index = function(self, key)
      for funcName, func in pairs(Components) do
         return Components[key][self.Id]
      end
      return rawget(self, key)
   end
}

function NewEntity(Id)
   local entity = {
      Id = Id
   }

   setmetatable(entity, MT)

   return entity;
end
