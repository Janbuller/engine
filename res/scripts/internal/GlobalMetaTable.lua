require("res/scripts/internal/Input")

setmetatable(_G, {
		__newindex = function (_G, key, val)
		   if key:sub(1, 2) == "g_" then
		      rawset(Entities["Globals"], key, val)
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

		   if key == "Entity" then
		      return rawget(Entities, EntityID)
		   end

		   return rawget(Entities[EntityID][ScriptID], key)
		end,
})

