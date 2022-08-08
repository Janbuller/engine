LastMousePos = Vec2:new(0)

Speed = 5;

Yaw = 0;
Pitch = 0;

function Init()
   LastMousePos.x = Input.GetMousePos(1);
   LastMousePos.y = Input.GetMousePos(2);

   local ET = Entity.Transform;

   ET.Position.x = -5;
   ET.Position.z = 5;

   Entity.Transform = ET;
end

function Update(dt)
   local MousePos = Vec2:new(0)
   MousePos.x = Input.GetMousePos(1);
   MousePos.y = Input.GetMousePos(2);

   local MousePosRelative = LastMousePos - MousePos;

   LastMousePos = MousePos;

   if(Input.KeyDown.LEFT_SHIFT) then
      Speed = 125;
   else
      Speed = 5;
   end

   local ET = Entity.Transform;

   Yaw += MousePosRelative.x * dt * 0.1;
   Pitch -= MousePosRelative.y * dt * 0.1;
   Pitch = math.min(math.max(-1.55, Pitch), 1.55)

   ET.Rotation = Quat:new(1, 0, 0, 0);
   ET.Rotation:Rotate(Pitch, Vec3.new(1, 0, 0));
   ET.Rotation:Rotate(Yaw, Vec3.new(0, -1, 0));

   local InvRot = ET.Rotation:Inverse();
   local Front = InvRot:RotatePoint(Vec3.new(0, 0, -1));
   local Right = InvRot:RotatePoint(Vec3.new(1, 0, 0));

   local Up = InvRot:RotatePoint(Vec3.new(0, 1, 0));

   if(Input.KeyDown.W) then
      ET.Position += Front * dt * Speed;
   end
   if(Input.KeyDown.S) then
      ET.Position -= Front * dt * Speed;
   end
   if(Input.KeyDown.A) then
      ET.Position -= Right * dt * Speed;
   end
   if(Input.KeyDown.D) then
      ET.Position += Right * dt * Speed;
   end
   if Input.KeyDown.E then
      ET.Position += Up * dt * Speed;
   end
   if Input.KeyDown.Q then
      ET.Position -= Up * dt * Speed;
   end
end

function Exit()
end
