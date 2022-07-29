local Quaternion = {}

function Quaternion:new(w, x, y, z)
   local quaternion = {}

   quaternion.w = w;
   quaternion.x = x;
   quaternion.y = y;
   quaternion.z = z;

   setmetatable(quaternion, self)
   self.__index = self
   return quaternion
end

function Quaternion:Rotate(Angle, Axis)
   RetQuat = Engine.Math.Rotate(self, Angle, Axis)
   self.w = RetQuat.w
   self.x = RetQuat.x
   self.y = RetQuat.y
   self.z = RetQuat.z
end

function Quaternion.RotatePoint(Quat, Point)
   local Rotated = Engine.Math.RotatePoint(Point, Quat)

   return Vector3:new(Rotated.x, Rotated.y, Rotated.z)
end

function Quaternion.Inverse(Quat)
   Inversed = Engine.Math.InverseQuat(Quat);

   return Quaternion:new(Inversed.w, Inversed.x, Inversed.y, Inversed.z);
end

return Quaternion
