require("res/Internal/Scripts/Input")

setmetatable(_G, {
		__newindex = function (_G, key, val)
		   if key:sub(1, 2) == "g_" then
		      rawset(Entities["Globals"], key, val)
		   end
		   if key:sub(1, 3) == "eg_" then
		      rawset(Entities[EntityID]["EntGlobals"], key, val)
		   end
		   if key == "Entity" then
		      rawset(Entities[EntityID], key, val)
		   end
		   rawset(Entities[EntityID][ScriptID], key, val)
		end,

		__index = function (_G, key)
		   if key:sub(1, 2) == "g_" then
		      return rawget(Entities["Globals"], key)
		   end

		   if key:sub(1, 3) == "eg_" then
		      return rawget(Entities[EntityID]["EntGlobals"], key)
		   end

		   if key == "Entity" then
		      return rawget(Entities, EntityID)
		   end

		   -- print(Entities, EntityID, ScriptID, key);
		   -- print( Entities[EntityID] );
		   return rawget(Entities[EntityID][ScriptID], key)
		end,
})

