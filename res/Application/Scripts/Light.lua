Time = 0;
Speed = 5;

function Init()
   -- NewId = MainScene.AddEntity()
   -- NewIdTransform = MainScene.AddComponent(NewId, "Transform");
   -- Position = Vec3.new(2, 3, 4)
   -- Rotation = Vec4.new(1, 0, 0, 0)
   -- Scale = Vec3(1, 1, 1));
end

function Update(dt)
   Time += dt * (Speed/10);
   local ToOne = (math.sin(Time)+1) / 2
   ToOne = 1;

   local EL = Entity.Light;

   EL.Intensity = ToOne * 5;
   -- EL.Intensity = 5;

   EL.Linear    = ((ToOne * -1) + 1) * 0.673 + 0.027
   EL.Quadratic = ((ToOne * -1) + 1) * 1.7972 + 0.0028
end

function Exit()
end
