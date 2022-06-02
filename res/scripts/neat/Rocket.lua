local Rocket = {}

local rocketPower = 35
local G = 30

function Rocket:new(x, y)
    local r = {}

    r.Goals = {
        {100, 100},
        {700, 100},
        {400, 300},
        {400, 500},
        {400, 100}
    }

    r.goalMarigineOfError = 30
    r.curGoal = 1

    r.globalR = 0
    r.globalRVel = 0
    r.x = x
    r.y = y
    r.xv = 0
    r.yv = -15

    r.boosterRotation = {0, 0}

    setmetatable(r, self)
    self.__index = self
    return r
end

function Rocket:update(dt, r1Left, r1Right, r2Left, r2Right, r1Boost, r2Boost)
    if r1Left then
        self.boosterRotation[1] = self.boosterRotation[1] - math.pi*dt
    end
    if r1Right then
        self.boosterRotation[1] = self.boosterRotation[1] + math.pi*dt
    end
    if r2Left then
        self.boosterRotation[2] = self.boosterRotation[2] - math.pi*dt
    end
    if r2Right then
        self.boosterRotation[2] = self.boosterRotation[2] + math.pi*dt
    end
    if r1Boost then
        local addX = math.cos(self.boosterRotation[1]-math.pi/2+self.globalR)*rocketPower*dt
        local addY = math.sin(self.boosterRotation[1]-math.pi/2+self.globalR)*rocketPower*dt

        local v = math.atan2(-35, 13)
        
        self.xv = self.xv + addX
        self.yv = self.yv + addY
        
        local v2 = math.atan2(-35+addX, 13+addY)

        if self.boosterRotation[1]-math.pi/2+self.globalR>0 then
            self.globalRVel = self.globalRVel - (v-v2)/2
        else
            self.globalRVel = self.globalRVel + (v-v2)/2
        end
    end
    if r2Boost then
        local addX = math.cos(self.boosterRotation[2]-math.pi/2+self.globalR)*rocketPower*dt
        local addY = math.sin(self.boosterRotation[2]-math.pi/2+self.globalR)*rocketPower*dt

        local v = math.atan2(35, 13)

        self.xv = self.xv + addX
        self.yv = self.yv + addY

        local v2 = math.atan2(35+addX, 13+addY)

        if self.boosterRotation[2]-math.pi/2+self.globalR>0 then
            self.globalRVel = self.globalRVel - (v-v2)/2
        else
            self.globalRVel = self.globalRVel + (v-v2)/2
        end
    end
    

    self.xv = self.xv - (self.xv * 0.50)*dt
    self.yv = self.yv + G*dt
    -- self.globalRVel = self.globalRVel - (self.globalRVel*0.50)*dt

    -- self.globalR = self.globalR + self.globalRVel
    
    -- if (self.globalR+math.pi/2)<0 then
    --     self.globalR = self.globalR + (math.pi*2)
    -- elseif (self.globalR+math.pi/2)>math.pi*2 then
    --     self.globalR = self.globalR - (math.pi*2)
    -- end

    self.x = self.x + self.xv
    self.y = self.y + self.yv
end

function Rocket:draw()
end

function Rocket:drawPoints()
    for _, g in pairs(self.Goals) do
    end
end

function Rocket:drawPointNLine()
    local g = self.Goals[self.curGoal]
end

return Rocket
