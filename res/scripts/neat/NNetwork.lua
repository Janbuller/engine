local Neuron = require("res/scripts/neat/Neuron")

local Network = {}

function Network:new(genome)
    local network = {}
    local pool = genome.pool
    network.pool = pool
	network.neurons = {}
	
	for i=1,pool.BaseInputs do
		network.neurons[i] = Neuron:new()
	end
	
	for o=1,pool.BaseOutputs do
		network.neurons[pool.defaults.MaxNodes+o] = Neuron:new()
	end
    
	table.sort(genome.genes, function (a,b)
		return (a.out < b.out)
	end)
	for i=1,#genome.genes do
		local gene = genome.genes[i]
		if gene.enabled then
			if network.neurons[gene.out] == nil then
				network.neurons[gene.out] = Neuron:new()
			end
			local neuron = network.neurons[gene.out]
			table.insert(neuron.incoming, gene)
			if network.neurons[gene.into] == nil then
				network.neurons[gene.into] = Neuron:new()
			end
		end
	end
    
    setmetatable(network, self)
    self.__index = self
    return network
end

function Network:evaluateNetwork(inputs)
	if #inputs ~= self.pool.BaseInputs then
		error("Incorrect number of neural network inputs.")
		return {}
	end
	
	for i=1,self.pool.BaseInputs do
		self.neurons[i].value = inputs[i]
	end
	
	for _,neuron in pairs(self.neurons) do
		local sum = 0
		for j = 1,#neuron.incoming do
			local incoming = neuron.incoming[j]
			local other = self.neurons[incoming.into]
			sum = sum + incoming.weight * other.value
		end
		
		if #neuron.incoming > 0 then
			neuron.value = sigmoid(sum)
		end
	end
	

	local outputs = {}
	for o=1,self.pool.BaseOutputs do
        if self.pool.outputAsBool then
            if self.neurons[self.pool.defaults.MaxNodes+o].value > 0 then
                outputs[o] = true
            else
                outputs[o] = false
            end
        else
            outputs[o] = self.neurons[self.pool.defaults.MaxNodes+o].value
        end
	end
	
	return outputs
end

function sigmoid(x)
	return 2/(1+math.exp(-4.9*x))-1
end

return Network
