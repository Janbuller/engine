local MT = {
   __index = function(self, key)
      for funcName, func in pairs(Components) do
	 if "Get"..key == funcName then
	    return func(self.Id)
	 end
      end
      return rawget(self, key)
   end,
   __newindex = function(self, key, v)
      for funcName, func in pairs(Components) do
	 if "Set"..key == funcName then
	    func(self.Id, v)
	    return;
	 end
      end
      rawset(self, key, v)
   end
}

function NewEntity(Id)
   local entity = {
      Id = Id
   }

   setmetatable(entity, MT)
   
   return entity;
end
