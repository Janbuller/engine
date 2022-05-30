time = 0;

function Init()
   time = math.random(1000)/10;
end

function Update(dt)
   time += dt;

   Entities[EntityID].Transform.Position.x = math.cos(time)*10;
   Entities[EntityID].Transform.Position.y = math.cos(time*1.42)*10;
   Entities[EntityID].Transform.Position.z = math.sin(time)*10;

   Entities[EntityID].Transform.Scale.x = (math.cos(time*4)*0.5) + 1
   Entities[EntityID].Transform.Scale.y = (math.cos(time*4)*0.5) + 1
   Entities[EntityID].Transform.Scale.z = (math.cos(time*4)*0.5) + 1
end

function Exit()
end
