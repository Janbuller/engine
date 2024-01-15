local Time = 0;
local StartHeight = {}
local Size = 32

local Quality = 8;

local Depth = 2

function Init()
   -- if not (EntityID == 0) then
   --    local model =  Entity.Model
   --    model.Visible = false
   --    Entity.Model = model
   -- end

   Time = math.random(1000) / 10;

   for x = 1, Size do
      StartHeight[x] = {}
      for z = 1, Size do
         StartHeight[x][z] = math.random(1000) / 1000
      end
   end

   local ET = Entity.Transform;

   ET.Position.x = -128
   ET.Position.z = -128

   ET.Scale.x = 64;
   ET.Scale.y = 64;
   ET.Scale.z = 64;

   ET.Rotation = Quat:new(1, 0, 0, 0)
end

function Update(dt)
   Time += dt;

   local EntModel = Entity.Model;

   local Mat = Material.new(EntModel.Meshes[1].Material);

   EntModel.Meshes[1] = Mesh.new();

   local Verts = {}

   local function Insert(Table, ToInsert)
      Table[#(Table) + 1] = ToInsert;
   end

   for x = 1, Size do
      for z = 1, Size do
         local vertex = Vertex.new()
         local pos = vertex.Position

         pos.x = (x - 1) / Quality;
         pos.y = StartHeight[x][z] * math.sin(Time * StartHeight[x][z]) / Quality
         -- pos.y = (math.min(0, math.sin(x+time)) * Depth) / Quality;
         pos.z = (z - 1) / Quality;

         vertex.Normal.x = 0;
         vertex.Normal.y = 1;
         vertex.Normal.z = 0;

         vertex.TexCoords.x = (x - 1) / (Size - 1);
         vertex.TexCoords.y = (z - 1) / (Size - 1);

         vertex.Tangent.x = 1
         vertex.Tangent.y = 0
         vertex.Tangent.z = 0

         if (x < Size) and (z < Size) then
            local i = (x - 1) + (z - 1) * Size;

            Insert(EntModel.Meshes[1].Indices, i + 1)
            Insert(EntModel.Meshes[1].Indices, i + Size)
            Insert(EntModel.Meshes[1].Indices, i)

            -- Norm = Vec3.new()
            -- Norm = Vec3.Cross(
            --    EntModel.Meshes[1].Vertices[i+size].Position - EntModel.Meshes[1].Vertices[i+1].Position,
            --    EntModel.Meshes[1].Vertices[i].Position      - EntModel.Meshes[1].Vertices[i+1].Position
            --    );

            -- EntModel.Meshes[1].Vertices[i].Normal = Norm;
            -- EntModel.Meshes[1].Vertices[i+1].Normal = Norm;
            -- EntModel.Meshes[1].Vertices[i+size].Normal = Norm;

            Insert(EntModel.Meshes[1].Indices, i + Size + 1)
            Insert(EntModel.Meshes[1].Indices, i + Size)
            Insert(EntModel.Meshes[1].Indices, i + 1)
         end

         table.insert(Verts, vertex)
      end
   end

   EntModel.Meshes[1].Vertices = Verts
   EntModel.Meshes[1].Material = Mat;
   EntModel.Meshes[1]:SetupBuffers()
end

function OnKeyPressed(Key, Action)

   -- if Key == Input.Keys["KEY_F"] then
   --    Log.Trace(Key .. " : " .. Action);
   -- end
   --
end

function Exit()
end

