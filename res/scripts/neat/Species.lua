local Species = {}

function Species:new(pool)
    local species = {}
    species.pool = pool
	species.topFitness = 0
	species.staleness = 0
	species.genomes = {}
	species.averageFitness = 0

    setmetatable(species, self)
    self.__index = self
    return species
end

function Species:calculateAverageFitness()
	local total = 0
	
	for g=1,#self.genomes do
		local genome = self.genomes[g]
		total = total + genome.globalRank
	end
	
	self.averageFitness = total / #self.genomes
end

function Species:breedChild()
	local child = {}
	if math.random() < self.pool.defaults.CrossoverChance then
		local g1 = self.genomes[math.random(1, #self.genomes)]
		local g2 = self.genomes[math.random(1, #self.genomes)]
		child = g1:crossover(g2)
	else
		local g = self.genomes[math.random(1, #self.genomes)]
		child = g:clone()
	end
	
	child:mutate()
	
	return child
end

return Species