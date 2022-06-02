Vector3 = {}

function Vector3:new(x, y, z)
  o = {}
  setmetatable(o, self)
  self.__index = self
  -- "operator overloading"
  self.__add = Vector3.add
  self.__sub = Vector3.subtract
  self.__mul = Vector3.multiply
  self.__div = Vector3.divide
  o.x = x
  o.y = y
  o.z = z
  return o
end

function Vector3.add(vector1, operand2)
  if type(operand2) == "table" then
    return Vector3:new(nil, vector1.x + operand2.x, vector1.y + operand2.y, vector1.z + operand2.z)
  elseif type(operand2) == "number" then
    return Vector3:new(nil, vector1.x + operand2, vector1.y + operand2, vector1.z + operand2)
  end
end

function Vector3.subtract(vector1, operand2)
  if type(operand2) == "table" then
    return Vector3:new(nil, vector1.x - operand2.x, vector1.y - operand2.y, vector1.z - operand2.z)
  elseif type(operand2) == "number" then
    return Vector3:new(nil, vector1.x - operand2, vector1.y - operand2, vector1.z - operand2)
  end
end

function Vector3.multiply(vector1, operand2)
  if type(operand2) == "table" then
    return Vector3:new(nil, vector1.x * operand2.x, vector1.y * operand2.y, vector1.z * operand2.z)
  elseif type(operand2) == "number" then
    return Vector3:new(nil, vector1.x * operand2, vector1.y * operand2, vector1.z * operand2)
  end
end

function Vector3.divide(vector1, operand2)
  if type(operand2) == "table" then
    return Vector3:new(nil, vector1.x / operand2.x, vector1.y / operand2.y, vector1.z / operand2.z)
  elseif type(operand2) == "number" then
    return Vector3:new(nil, vector1.x / operand2, vector1.y / operand2, vector1.z / operand2)
  end
end

function Vector3.length(vector)
  return math.sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z)
end

function Vector3.normalize(vector)
  local len = Vector3.length(vector)
  return Vector3.divide(vector, Vector3:new(nil, len, len, len))
end

function Vector3.cross(vector1, vector2)
    result = Vector3:new(nil, 0, 0, 0)
    result.x = l1.y * l2.z - l1.z * l2.y
    result.y = l1.z * l2.x - l1.x * l2.z
    result.z = l1.x * l2.y - l1.y * l2.x
    return result
end

-- The sum of all components of a vector
function Vector3.sum(vector)
  return vector.x + vector.y + vector.z
end

function Vector3:setX(x)
  self.x = x
end

function Vector3:setY(y)
  self.y = y
end

function Vector3:setZ(z)
  self.z = z
end

function Vector3:set(index, value)
  if index == 1
  then self.x = value
  elseif index == 2
  then self.y = value
  elseif index == 3
  then self.z = value
end
end

function Vector3:print()
    print(self.x, self.y, self.z)
end

return Vector3
