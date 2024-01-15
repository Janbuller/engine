Time = 0;
Speed = 5;

function Init()
   -- NewId = MainScene.AddEntity()
   -- NewIdTransform = MainScene.AddComponent(NewId, "Transform");
   -- Position = Vec3.new(2, 3, 4)
   -- Rotation = Vec4.new(1, 0, 0, 0)
   -- Scale = Vec3(1, 1, 1));
   local ET = Entity.Transform
   local EL = Entity.Light;

   EL.Intensity = 5;
   -- EL.Intensity = 5;

   EL.Linear    = ((1 * -1) + 1) * 0.673 + 0.027
   EL.Quadratic = ((1 * -1) + 1) * 1.7972 + 0.0028

   EL.Color = Vec3:new(math.random(), math.random(), math.random());

   ET.Position = Vec3:new(math.random()*200-100, 1, math.random()*200-100)
   StartPos = ET.Position;

   Time = math.random() * math.pi
end

function Update(dt)
   Time += dt

   local ET = Entity.Transform
   ET.Position.x = StartPos.x + math.cos(Time)/2
   ET.Position.z = StartPos.z + math.sin(Time)/2
end

function Exit()
end
