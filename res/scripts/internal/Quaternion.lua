local Quaternion = {}

function Quaternion:new(x, y, z, w)
   local quaternion = {}

   quaternion.x = x;
   quaternion.y = y;
   quaternion.z = z;
   quaternion.w = w;

   setmetatable(quaternion, self)
   self.__index = self
   return quaternion
end

function Quaternion:Rotate(Angle, Axis)
   local RotTable = {
      self.x,
      self.y,
      self.z,
      self.w
   }

   local AxisTable = {
      Axis.x,
      Axis.y,
      Axis.z
   }

   RotTable = GMath.Rotate(RotTable, Angle, AxisTable)

   self.x = RotTable[1]
   self.y = RotTable[2]
   self.z = RotTable[3]
   self.w = RotTable[4]
end

function Quaternion.RotatePoint(Quat, Point)
   local RotTable = {
      Quat.x,
      Quat.y,
      Quat.z,
      Quat.w
   }

   local PointTable = {
      Point.x,
      Point.y,
      Point.z
   }

   local Rotated = GMath.RotatePoint(PointTable, RotTable)


   Rotated = Vector3:new(Rotated[1], Rotated[2], Rotated[3]);
   return Rotated
end

function Quaternion.Inverse(Quat)
   local QuatTable = {
      Quat.x,
      Quat.y,
      Quat.z,
      Quat.w
   }

   QuatTable = GMath.InverseQuat(QuatTable);

   return Quaternion:new(QuatTable[1], QuatTable[2], QuatTable[3], QuatTable[4]);
end

return Quaternion
