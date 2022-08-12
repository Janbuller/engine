Time = 0;
Speed = 5;

function Init()
end

function Update(dt)
   Time += dt * (Speed/10);
   local ToOne = (math.sin(Time)+1) / 2

   local EL = Entity.Light;

   EL.Intensity = ToOne * 5;
   -- EL.Intensity = 5;

   EL.Linear    = ((ToOne * -1) + 1) * 0.673 + 0.027
   EL.Quadratic = ((ToOne * -1) + 1) * 1.7972 + 0.0028
end

function Exit()
end
