local Gene = require("res/scripts/neat/Gene")
local Genome = {}

function Genome:new(pool)
    local genome = {}
	genome.genes = {}
	genome.time = pool.Time
	genome.fitness = 0
	genome.adjustedFitness = 0
	genome.network = {}
	genome.maxneuron = 0
	genome.globalRank = 0
	genome.mutationRates = {}
	genome.mutationRates["connections"] = pool.defaults.MutateConnectionsChance
	genome.mutationRates["link"] = pool.defaults.LinkMutationChance
	genome.mutationRates["bias"] = pool.defaults.BiasMutationChance
	genome.mutationRates["node"] = pool.defaults.NodeMutationChance
	genome.mutationRates["enable"] = pool.defaults.EnableMutationChance
	genome.mutationRates["disable"] = pool.defaults.DisableMutationChance
	genome.mutationRates["step"] = pool.defaults.StepSize

	genome.instance = pool.newInstance()

    genome.pool = pool

	genome.maxneuron = pool.BaseInputs

    setmetatable(genome, self)
    self.__index = self
    
	genome:mutate()
    return genome
end

function Genome:evaluate(inputs)
    return self.network:evaluateNetwork(inputs)
end

function Genome:sameSpecies(genome2)
    local genome1 = self
	local dd = self.pool.defaults.DeltaDisjoint*Genome.disjoint(genome1.genes, genome2.genes)
	local dw = self.pool.defaults.DeltaWeights*Genome.weights(genome1.genes, genome2.genes) 
	return dd + dw < self.pool.defaults.DeltaThreshold
end

function Genome:weights(genes2)
    local genes1 = self
	local i2 = {}
	for i = 1,#genes2 do
		local gene = genes2[i]
		i2[gene.innovation] = gene
	end

	local sum = 0
	local coincident = 0
	for i = 1,#genes1 do
		local gene = genes1[i]
		if i2[gene.innovation] ~= nil then
			local gene2 = i2[gene.innovation]
			sum = sum + math.abs(gene.weight - gene2.weight)
			coincident = coincident + 1
		end
	end
	
	return sum / coincident
end

function Genome:disjoint(genes2)
    local genes1 = self
	local i1 = {}
	for i = 1,#genes1 do
		local gene = genes1[i]
		i1[gene.innovation] = true
	end

	local i2 = {}
	for i = 1,#genes2 do
		local gene = genes2[i]
		i2[gene.innovation] = true
	end
	
	local disjointGenes = 0
	for i = 1,#genes1 do
		local gene = genes1[i]
		if not i2[gene.innovation] then
			disjointGenes = disjointGenes+1
		end
	end
	
	for i = 1,#genes2 do
		local gene = genes2[i]
		if not i1[gene.innovation] then
			disjointGenes = disjointGenes+1
		end
	end
	
	local n = math.max(#genes1, #genes2)
	
	return disjointGenes / n
end

function Genome:mutate()
	for mutation,rate in pairs(self.mutationRates) do
		if math.random(1,2) == 1 then
			rate = 0.95*rate
		else
			rate = (1/0.95)*rate
		end
	end

	if math.random() < self.mutationRates["connections"] then
		self:pointMutate()
	end
	
	local p = self.mutationRates["link"]
	while p > 0 do
		if math.random() < p then
			self:linkMutate(false)
		end
		p = p - 1
	end

	p = self.mutationRates["bias"]
	while p > 0 do
		if math.random() < p then
			self:linkMutate(true)
		end
		p = p - 1
	end
	
	p = self.mutationRates["node"]
	while p > 0 do
		if math.random() < p then
			self:nodeMutate()
		end
		p = p - 1
	end
	
	p = self.mutationRates["enable"]
	while p > 0 do
		if math.random() < p then
			self:enableDisableMutate(true)
		end
		p = p - 1
	end

	p = self.mutationRates["disable"]
	while p > 0 do
		if math.random() < p then
			self:enableDisableMutate(false)
		end
		p = p - 1
	end
end

function Genome:pointMutate()
	local step = self.mutationRates["step"]
	
	for i=1,#self.genes do
		local gene = self.genes[i]
		if math.random() < self.pool.defaults.PerturbChance then
			gene.weight = gene.weight + math.random() * step*2 - step
		else
			gene.weight = math.random()*4-2
		end
	end
end

function Genome:linkMutate(forceBias)
	local neuron1 = self:randomNeuron(false)
	local neuron2 = self:randomNeuron(true)
	 
	local newLink = Gene:new()
	if neuron1 <= self.pool.BaseInputs and neuron2 <= self.pool.BaseInputs then
		--Both input nodes
		return
	end
	if neuron2 <= self.pool.BaseInputs then
		-- Swap output and input
		local temp = neuron1
		neuron1 = neuron2
		neuron2 = temp
	end

	newLink.into = neuron1
	newLink.out = neuron2
	if forceBias then
		newLink.into = self.pool.BaseInputs
	end
	
	if self:containsLink(newLink) then
		return
	end
	newLink.innovation = self.pool:newInnovation()
	newLink.weight = math.random()*4-2
	
	table.insert(self.genes, newLink)
end

function Genome:containsLink(link)
	for i=1,#self.genes do
		local gene = self.genes[i]
		if gene.into == link.into and gene.out == link.out then
			return true
		end
	end
end

function Genome:nodeMutate()
	if #self.genes == 0 then
		return
	end

	self.maxneuron = self.maxneuron + 1

	local gene = self.genes[math.random(1,#self.genes)]
	if not gene.enabled then
		return
	end
	gene.enabled = false
	
	local gene1 = gene:clone()
	gene1.out = self.maxneuron
	gene1.weight = 1.0
	gene1.innovation = self.pool:newInnovation()
	gene1.enabled = true
	table.insert(self.genes, gene1)
	
	local gene2 = gene:clone()
	gene2.into = self.maxneuron
	gene2.innovation = self.pool:newInnovation()
	gene2.enabled = true
	table.insert(self.genes, gene2)
end

function Genome:enableDisableMutate(enable)
	local candidates = {}
	for _,gene in pairs(self.genes) do
		if gene.enabled == not enable then
			table.insert(candidates, gene)
		end
	end
	
	if #candidates == 0 then
		return
	end
	
	local gene = candidates[math.random(1,#candidates)]
	gene.enabled = not gene.enabled
end

function Genome:randomNeuron(nonInput)
	local neurons = {}
	if not nonInput then
		for i=1,self.pool.BaseInputs do
			neurons[i] = true
		end
	end
	for o=1,self.pool.BaseOutputs do
		neurons[self.pool.defaults.MaxNodes+o] = true
	end
	for i=1,#self.genes do
		if (not nonInput) or self.genes[i].into > self.pool.BaseInputs then
			neurons[self.genes[i].into] = true
		end
		if (not nonInput) or self.genes[i].out > self.pool.BaseInputs then
			neurons[self.genes[i].out] = true
		end
	end

	local count = 0
	for _,_ in pairs(neurons) do
		count = count + 1
	end
	local n = math.random(1, count)
	
	for k,v in pairs(neurons) do
		n = n-1
		if n == 0 then
			return k
		end
	end
	
	return 0
end

function Genome:crossover(g2)
	local g1 = self
	-- Make sure g1 is the higher fitness genome
	if g2.fitness > g1.fitness then
		tempg = g1
		g1 = g2
		g2 = tempg
	end

	local child = Genome:new(self.pool)
	
	local innovations2 = {}
	for i=1,#g2.genes do
		local gene = g2.genes[i]
		innovations2[gene.innovation] = gene
	end
	
	for i=1,#g1.genes do
		local gene1 = g1.genes[i]
		local gene2 = innovations2[gene1.innovation]
		if gene2 ~= nil and math.random(2) == 1 and gene2.enabled then
			table.insert(child.genes, gene2:clone())
		else
			table.insert(child.genes, gene1:clone())
		end
	end
	
	child.maxneuron = math.max(g1.maxneuron,g2.maxneuron)
	
	for mutation,rate in pairs(g1.mutationRates) do
		child.mutationRates[mutation] = rate
	end
	
	return child
end

function Genome:clone()
	local genome2 = Genome:new(self.pool)
	for g=1,#self.genes do
		table.insert(genome2.genes, self.genes[g]:clone())
	end
	genome2.maxneuron = self.maxneuron
	genome2.mutationRates["connections"] = self.mutationRates["connections"]
	genome2.mutationRates["link"] = self.mutationRates["link"]
	genome2.mutationRates["bias"] = self.mutationRates["bias"]
	genome2.mutationRates["node"] = self.mutationRates["node"]
	genome2.mutationRates["enable"] = self.mutationRates["enable"]
	genome2.mutationRates["disable"] = self.mutationRates["disable"]
	
	return genome2
end

return Genome
