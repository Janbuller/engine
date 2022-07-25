time = 0;

StartHeight = {}
size = 8

Quality = 1;

print("hello, world!", 52)

function Init()
   -- if not (EntityID == 0) then
   --    local model =  Entity.Model
   --    model.Visible = false
   --    Entity.Model = model
   -- end

   -- time = math.random(1000)/10;

   -- for x = 1, size do
   --    StartHeight[x] = {}
   --    for z = 1, size do
   --       StartHeight[x][z] = math.random(1000)/1000
   --    end
   -- end

   local ET = Entity.Transform;

   ET.Scale.x = 64;
   ET.Scale.y = 64;
   ET.Scale.z = 64;

   Entity.Transform = ET;


   -- time += dt;

   -- local EntModel = Model:new();
   -- EntModel.Meshes[1] = Mesh:new();

   -- local Verts = {}

   -- for x = 1, size do
   --    for z = 1, size do
   --       local vertex = Vertex:new()
   --       local pos = vertex.Position

   --       pos.x = (x-1) / Quality;
   --       -- pos.y = StartHeight[x][z] * math.cos(time*StartHeight[x][z])
   --       pos.y = math.min(0, math.sin(x+time));
   --       pos.z = (z-1) / Quality;

   --       vertex.Normal.x = -1;
   --       vertex.Normal.y = 0;
   --       vertex.Normal.z = 0;

   --       vertex.TexCoords.x = (x-1)/(size-1);
   --       vertex.TexCoords.y = (z-1)/(size-1);

   --       if(x < size) and (z < size) then
   --          local i = (x-1) + (z-1) * size;
   --          table.insert(EntModel.Meshes[1].Indices, i)
   --          table.insert(EntModel.Meshes[1].Indices, i+size)
   --          table.insert(EntModel.Meshes[1].Indices, i+1)
   --          table.insert(EntModel.Meshes[1].Indices, i+1)
   --          table.insert(EntModel.Meshes[1].Indices, i+size)
   --          table.insert(EntModel.Meshes[1].Indices, i+size+1)
   --       end

   --       table.insert(Verts, vertex)
   --    end
   -- end

   -- EntModel.Meshes[1].Vertices = Verts

   -- Entity.Model = EntModel
end

function Update(dt)
end

function OnKeyPressed(Key, Action)

   -- if Key == Input.Keys["KEY_F"] then
   --    Log.Trace(Key .. " : " .. Action);
   -- end
end

function Exit()
end

