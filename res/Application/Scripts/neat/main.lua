local Rocket = require("res/scripts/neat/Rocket")
NEAT = require("res/scripts/neat/NEAT"):new(
    12, -- x, y, xv, xy, r1, r2, Gr, rV, goalX, goalY, nextGoalX, nextGoalY
    6,
    100,
    true,
    4,
    function ()
        local R = Rocket:new(400, 300)
        R.fitness = 0
        return R
    end,
    function (R)
        local g = R.Goals[R.curGoal]
        local g2 = R.Goals[R.curGoal+1]
        if not g2 then
            g2 = g
        end
        return {
            R.x,
            R.y,
            R.xv,
            R.yv,
            R.boosterRotation[1],
            R.boosterRotation[2],
            R.globalR,
            R.globalRVel,
            (R.x-g[1]),
            (R.y-g[2]),
            (R.x-g2[1]),
            (R.y-g2[2])
        }
    end,
    function (R, output, dt, time, inputs) -- genome.instance, output, dt, genome.time, inputs
        R:update(
            dt,
            output[1],
            output[2],
            output[3],
            output[4],
            output[5],
            output[6]
        )
        R.fitness = R.fitness + dt*10

        R.fitness = R.fitness + (math.pi*2-math.abs(R.globalR))*20*dt
        
        local g = R.Goals[R.curGoal]
        if ((g[1]-R.x)^2+(g[2]-R.y)^2)^0.5 < R.goalMarigineOfError then
            if R.Goals[R.curGoal+1] then
                R.curGoal = R.curGoal + 1
            else
                R.fitness = 100^100
                return R, 0
            end
            R.fitness = R.fitness + 2000
            return R, time + 3
        end
        if ((400-R.x)^2+(300-R.y)^2)^0.5 > 600 then
            R.fitness = R.fitness - 10000
            return R, 0
        end
        return R
    end,
    function (R)
        local g = R.Goals[R.curGoal]
        local distFitness = 600-((g[1]-R.x)^2+(g[2]-R.y)^2)^0.5
        return R.fitness + distFitness/100
    end
)

local myR = Rocket:new(400, 300)

math.randomseed(os.time())

NEAT:setupNNs()

NNRockets = {}

function Update(dt)

   for i = 0,99 do
      Entities[i].Model.Visible = false
   end

   local idx = 0
    for R in NEAT:getActiveInstances() do
       Entities[idx].Transform.Position = {x = R.x, y = -R.y, z = 0}
       Entities[idx].Model.Visible = true
       idx += 1
    end

    NEAT:update(1/60)
end

