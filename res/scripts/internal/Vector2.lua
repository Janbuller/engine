Vector2 = {x = 0, y = 0}

function Vector2:new(x, y)
  o = {}
  setmetatable(o, self)
  self.__index = self
  -- "operator overloading"
  self.__add = Vector2.add
  self.__sub = Vector2.subtract
  self.__mul = Vector2.multiply
  self.__div = Vector2.divide

  o.x = x
  o.y = y

  return o
end

function Vector2.add(vector1, vector2)
  return Vector2:new(vector1.x + vector2.x, vector1.y + vector2.y)
end

function Vector2.subtract(vector1, vector2)
  return Vector2:new(vector1.x - vector2.x, vector1.y - vector2.y)
end

function Vector2.multiply(vector1, vector2)
  return Vector2:new(vector1.x * vector2.x, vector1.y * vector2.y)
end

function Vector2.divide(vector1, vector2)
  return Vector2:new(vector1.x / vector2.x, vector1.y / vector2.y)
end

function Vector2.length(vector)
  return math.sqrt(vector.x * vector.x + vector.y * vector.y)
end

function Vector2.normalize(vector)
  local len = Vector2:length(vector)
  return Vector2:divide(vector, Vector2:new(len, len))
end

return Vector2
