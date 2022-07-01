function NewNN(LayerThingy, range)
    Layers = LayerThingy
    for i = 1,#Layers do
        saveVal = Layers[i]
        Layers[i] = {}
        for i2 = 1,saveVal do
            if i == 1 then
                Layers[i][i2] = {1, {}}
            else
                Layers[i][i2] = {1, {}}
                for i3 = 1,#Layers[i-1] do
                    table.insert(Layers[i][i2][2], math.random(-range*1000, range*1000)/1000)
                end
            end
        end
    end
    return Layers
end

function LineNLineCol(x1, y1, x2, y2, x3, y3, x4, y4)
    uA = ((x4-x3)*(y1-y3) - (y4-y3)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1)); uB = ((x2-x1)*(y1-y3) - (y2-y1)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1))
    if uA >= 0 and uA <= 1 and uB >= 0 and uB <= 1 then
        return {x1 + (uA * (x2-x1)), y1 + (uA * (y2-y1))}
    end
    return false
end

function deepcopy(orig)
    local orig_type = type(orig)
    local copy
    if orig_type == 'table' then
        copy = {}
        for orig_key, orig_value in next, orig, nil do
            copy[deepcopy(orig_key)] = deepcopy(orig_value)
        end
        setmetatable(copy, deepcopy(getmetatable(orig)))
    else -- number, string, boolean, etc
        copy = orig
    end
    return copy
end

function trainNN(LayersList)
    for i = #LayersList, 1, -1 do
        if i ~= 0 then
            for i2 = 1,#LayersList[i] do
                for i3 = 1,#LayersList[i][i2][2] do
                    LayersList[i][i2][2][i3] = LayersList[i][i2][2][i3] - 0.01*(LayersList[i][i2][1]-0.5)
                end
            end
        end
    end
    return LayersList
end

function updateNN(LayersList, input, biaz)
    biaz = biaz or 0
    for i = 1,#LayersList[1] do
        LayersList[1][i][1] = input[i]
    end
    for i = 1,#LayersList do
        if i ~= 1 then
            for i2 = 1,#LayersList[i] do
                totalVal = biaz
                for i3 = 1,#LayersList[i][i2][2] do
                    totalVal = totalVal + LayersList[i-1][i3][1]*LayersList[i][i2][2][i3]
                end
                --totalVal = totalVal/#LayersList[i][i2][2]
                LayersList[i][i2][1] = 1/(1+math.exp(1)^(-totalVal))
            end
        end
    end
    NVal = {0, 0}
    for i = 1,#LayersList[#LayersList] do
        if NVal[1] < LayersList[#LayersList][i][1] then
            NVal = {LayersList[#LayersList][i][1], i}
        end
    end
    return LayersList[#LayersList]
end

function DrawNN(LayersList)
    for i = 1,#LayersList do
        for i2 = 1,#LayersList[i] do
            love.graphics.setColor(1, 1, 1, LayersList[i][i2][1])
            love.graphics.circle("fill", 50+150*(i-1), h/2-(i2-#LayersList[i]/2)*50, 15)
            if i ~= 1 then
                for i3 = 1,#LayersList[i][i2][2] do
                    love.graphics.setColor(1, 1, 1, (LayersList[i][i2][2][i3]+1)/2)
                    love.graphics.line(50+150*(i-1), h/2-(i2-#LayersList[i]/2)*50, 50+150*(i-2), h/2-(i3-#LayersList[i-1]/2)*50)
                end
            end
        end
    end
end
