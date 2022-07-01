local Neuron = {}

function Neuron:new()
    local neuron = {}
	neuron.incoming = {}
	neuron.value = 0.0
    
    setmetatable(neuron, self)
    self.__index = self
    return neuron
end

return Neuron