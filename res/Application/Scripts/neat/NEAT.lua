local Genome = require("res/scripts/neat/Genome")
local Gene = require("res/scripts/neat/Gene")
local Species = require("res/scripts/neat/Species")
local NNetwork = require("res/scripts/neat/NNetwork")

local Pool = {defaults={
    MutateConnectionsChance = 0.25,
    LinkMutationChance = 2.0,
    BiasMutationChance = 0.40,
    NodeMutationChance = 0.50,
    EnableMutationChance = 0.2,
    DisableMutationChance = 0.4,
    StepSize = 0.1,
    MaxNodes = 1000000,
    DeltaDisjoint = 2.0,
    DeltaWeights = 0.4,
    DeltaThreshold = 1.0,
    StaleSpecies = 15,
    CrossoverChance = 0.75,
    PerturbChance = 0.90,
}}

function Pool:new(Inputs, Outputs, Population, outputAsBool, Time, newInstance, getInputs, updateInstance, calcFitness)
    local pool = {}
	pool.species = {}
	pool.generation = 0
	pool.innovation = Outputs
	pool.currentSpecies = 1
	pool.currentGenome = 1
	pool.currentFrame = 0
    pool.bestNN = nil
	pool.maxFitness = 0
    pool.BaseInputs = Inputs
    pool.BaseOutputs = Outputs


    pool.outputAsBool = outputAsBool
    pool.Time = Time
    pool.getInputs = getInputs
    pool.newInstance = newInstance
    pool.updateInstance = updateInstance
    pool.calcFitness = calcFitness

    pool.Population = Population
    pool.lastOutput = nil

    setmetatable(pool, self)
    self.__index = self

    -- add genomes, think they are like all instances of brains... but probly not...
	for i=1,Population do
		local basic = Genome:new(pool)
		pool:addToSpecies(basic)
	end

    return pool
end

function Pool:update(dt)
    local didUpdate = false

    for _, species in pairs(self.species) do
        for _, genome in pairs(species.genomes) do
            if genome.time > 0 then
                didUpdate = true

                local inputs = self.getInputs(genome.instance)

                local output = genome:evaluate(inputs)
                local saveTime = genome.time
                genome.instance, genome.time = self.updateInstance(genome.instance, output, dt, genome.time, inputs)
                if genome.time == nil then
                    genome.time = saveTime - dt
                end
            end
        end
    end

    if not didUpdate then
        self:internalCalcFitness()
        self:newGeneration()
        self:setupNNs()
    end

    return didUpdate
end

function Pool:internalCalcFitness()
    for _, species in pairs(self.species) do
        for _, genome in pairs(species.genomes) do
            genome.fitness = self.calcFitness(genome.instance)
            if genome.fitness > self.maxFitness then
                self.maxFitness = genome.fitness
                self.bestNN = genome.network
            end
			genome.time = self.Time
        end
    end
end

function Pool:evalBestNet(input)
    if self.bestNN then
        return self.bestNN:evaluateNetwork(input) 
    end
    return {0}
end

function Pool:skipXGenerationsWithDT(x, dt)
	local timeStart = os.time()
	local gen = self.generation
	local lastGen = self.generation
	while self.generation < gen+x do
		self:update(dt)
		if self.generation ~= lastGen then
			lastGen = self.generation
			print("Now starting generation: " .. lastGen)
		end
	end
	print("Time passed for gen skip: "..os.time()-timeStart)
end

function Pool:setupNNs()
    for _, species in pairs(self.species) do
        for _, genome in pairs(species.genomes) do
            genome.network = NNetwork:new(genome)
        end
    end
end

function Pool:newInnovation()
	self.innovation = self.innovation + 1
	return self.innovation
end

function Pool:addToSpecies(genomeToAdd)
	local foundSpecies = false
	for s, species in pairs(self.species) do
		if not foundSpecies and Genome.sameSpecies(genomeToAdd, species.genomes[1]) then
			table.insert(species.genomes, genomeToAdd)
			foundSpecies = true
		end
	end
	
	if not foundSpecies then
		local childSpecies = Species:new(self)
		table.insert(childSpecies.genomes, genomeToAdd)
		table.insert(self.species, childSpecies)
	end
end

function Pool:cullSpecies(cutToOne)
	for s = 1,#self.species do
		local species = self.species[s]
		
		table.sort(species.genomes, function (a,b)
			return (a.fitness > b.fitness)
		end)
		
		local remaining = math.ceil(#species.genomes/2)
		if cutToOne then
			remaining = 1
		end
		while #species.genomes > remaining do
			table.remove(species.genomes)
		end
	end
end

function Pool:rankGlobally()
	local global = {}
	for s = 1, #self.species do
		local species = self.species[s]
		for g = 1,#species.genomes do
			table.insert(global, species.genomes[g])
		end
	end
	table.sort(global, function (a,b)
		return (a.fitness < b.fitness)
	end)
	
	for g=1,#global do
		global[g].globalRank = g
	end
end

function Pool:removeStaleSpecies()
	local survived = {}

	for s = 1,#self.species do
		local species = self.species[s]
		
		table.sort(species.genomes, function (a,b)
			return (a.fitness > b.fitness)
		end)
		
		if species.genomes[1].fitness > species.topFitness then
			species.topFitness = species.genomes[1].fitness
			species.staleness = 0
		else
			species.staleness = species.staleness + 1
		end
		if species.staleness < self.defaults.StaleSpecies or species.topFitness >= self.maxFitness then
			table.insert(survived, species)
		end
	end

	self.species = survived
end

function Pool:removeWeakSpecies()
	local survived = {}

	local sum = self:totalAverageFitness()
	for s = 1,#self.species do
		local species = self.species[s]
		local breed = math.floor(species.averageFitness / sum * self.Population)
		if breed >= 1 then
			table.insert(survived, species)
		end
	end

	self.species = survived
end

function Pool:totalAverageFitness()
	local total = 0
	for s = 1,#self.species do
		local species = self.species[s]
		total = total + species.averageFitness
	end

	return total
end

function Pool:averageTotalAverageFitness()
	local total = 0
    local instances = 0
	for s = 1,#self.species do
        instances = instances + 1
		local species = self.species[s]
		total = total + species.averageFitness
	end

	return total/instances
end

function Pool:newGeneration()
	self:cullSpecies(false) -- Cull the bottom half of each species
	self:rankGlobally()
	self:removeStaleSpecies()
	self:rankGlobally()
	for s = 1,#self.species do
		local species = self.species[s]
		species:calculateAverageFitness()
	end
	self:removeWeakSpecies()
	local sum = self:totalAverageFitness()
	local children = {}
	for s = 1,#self.species do
		local species = self.species[s]
		local breed = math.floor(species.averageFitness / sum * self.Population) - 1
		for i=1,breed do
			table.insert(children, species:breedChild())
		end
	end
	self:cullSpecies(true) -- Cull all but the top member of each species
	while #children + #self.species < self.Population do
		local species = self.species[math.random(1, #self.species)]
		table.insert(children, species:breedChild())
	end
	for c=1,#children do
		local child = children[c]
		self:addToSpecies(child)
	end
	
	self.generation = self.generation + 1
end

function Pool:getActiveInstances()
    local instances = {}
    local i = 0
    for _, species in pairs(self.species) do
        for _, genome in pairs(species.genomes) do
            if genome.time > 0 then
                table.insert(instances, genome.instance)
            end
        end
    end

    return function()
        i = i + 1
        return instances[i]
    end
end

function Pool:getActiveInstancesCount()
	local index = 0
	for i in self:getActiveInstances() do
		index = index + 1
	end
	return index
end

function Pool:getInstances()
    local instances = {}
    local i = 0
    for _, species in pairs(self.species) do
        for _, genome in pairs(species.genomes) do
            table.insert(instances, genome.instance)
        end
    end

    return function()
        i = i + 1
        return instances[i]
    end
end

function Pool:getInstancesCount()
	local index = 0
	for i in self:getInstances() do
		index = index + 1
	end
	return index
end

return Pool
