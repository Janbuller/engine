time = 0;

StartHeight = {}
size = 8

function Init()
   if not (EntityID == 0) then
      local model =  Entity.Model
      model.Visible = false
      Entity.Model = model
   end

   time = math.random(1000)/10;

   for x = 1, size do
      StartHeight[x] = {}
      for z = 1, size do
         StartHeight[x][z] = math.random(1000)/1000
      end
   end

   local ET = Entity.Transform;
   ET.Position.z -= 5;
   ET.Position.y -= 2.5;
   ET.Position.x += 2.5;

   ET.Rotation:Rotate(90, Vector3:new(0, 0, 1));

   Entity.Transform = ET;
end

function Update(dt)
   time += dt;

   local EntModel = Model:new();
   EntModel.Meshes[1] = Mesh:new();

   local Verts = {}

   for x = 1, size do
      for z = 1, size do
         local vertex = Vertex:new()
         local pos = vertex.Position

         pos.x = x-1
         pos.y = StartHeight[x][z] * math.cos(time*StartHeight[x][z])
         pos.z = z-1

         vertex.Normal.x = -1;
         vertex.Normal.y = 0;
         vertex.Normal.z = 0;

         vertex.TexCoords.x = (x-1)/(size-1);
         vertex.TexCoords.y = (z-1)/(size-1);

         for i = 1, (size-1)^2*2 do
            table.insert(EntModel.Meshes[1].Indicies, i)
            table.insert(EntModel.Meshes[1].Indicies, i+1)
            table.insert(EntModel.Meshes[1].Indicies, i+8)
            table.insert(EntModel.Meshes[1].Indicies, i+9)
            table.insert(EntModel.Meshes[1].Indicies, i+1)
            table.insert(EntModel.Meshes[1].Indicies, i+8)
         end
         table.insert(Verts, vertex)
      end
   end

   EntModel.Meshes[1].Vertices = Verts

   Entity.Model = EntModel
end

function OnKeyPressed(Key, Action)
   -- if Key == Input.Keys["KEY_F"] then
   --    Log.Trace(Key .. " : " .. Action);
   -- end
end

function Exit()
end

