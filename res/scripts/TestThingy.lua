time = 0;

function Init()
   time = math.random(1000)/10;
end

function Update(dt)
   time += dt;

   Entity.Transform.Position.x = math.cos(time)*(Entity.Transform.Position.y * 0.5 + 1);
   Entity.Transform.Position.y = math.cos(time*1.42)*20 + 20;
   Entity.Transform.Position.z = math.sin(time)*(Entity.Transform.Position.y * 0.5 + 1);

   Entity.Transform.Rotation:Rotate(dt * math.random(100)/100, Vector3:new(0, 1, 0));

   Entity.Transform.Scale.x = (math.cos(time*4)*0.5) + 1
   Entity.Transform.Scale.y = (math.cos(time*4)*0.5) + 1
   Entity.Transform.Scale.z = (math.cos(time*4)*0.5) + 1

   if(Input.KeyDown.F) then
      time += dt * 4
   end

end

function OnKeyPressed(Key, Action)
   -- if Key == Input.Keys["KEY_F"] then
   --    Log.Trace(Key .. " : " .. Action);
   -- end
end

function Exit()
end

