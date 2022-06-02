time = 0;

function Init()
   time = math.random(1000)/10;

end

function Update(dt)
   time += dt;

   local Trans = Entity.Transform;

   Trans.Position.x = math.cos(time)*(Trans.Position.y * 0.5 + 1);
   Trans.Position.y = math.cos(time*1.42)*20 + 20;
   Trans.Position.z = math.sin(time)*(Trans.Position.y * 0.5 + 1);

   Trans.Rotation:Rotate(dt * math.random(100)/100, Vector3:new(0, 1, 0));
   Trans.Rotation:Rotate(dt * math.random(100)/100, Vector3:new(1, 0, 0));

   Trans.Scale.x = (math.cos(time*4)*0.5) + 1
   Trans.Scale.y = (math.cos(time*4)*0.5) + 1
   Trans.Scale.z = (math.cos(time*4)*0.5) + 1

   -- Entity.Transform = Trans

   local EntModel = Entity.Model;
   EntModel.Meshes[1].Vertices[7].Position = Vector3:new(math.sin(time)*2, 1.0, math.cos(time)*2);
   EntModel.Meshes[1].Vertices[11].Position = Vector3:new(math.sin(time)*2, 1.0, math.cos(time)*2);
   Entity.Model = EntModel;

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

