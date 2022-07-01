local Gene = {}

function Gene:new()
    local gene = {}
	gene.into = 0
	gene.out = 0
	gene.weight = 0.0
	gene.enabled = true
	gene.innovation = 0

    setmetatable(gene, self)
    self.__index = self
    return gene
end

function Gene:clone()
	local gene2 = Gene:new()
	gene2.into = self.into
	gene2.out = self.out
	gene2.weight = self.weight
	gene2.enabled = self.enabled
	gene2.innovation = self.innovation
	
	return gene2
end

return Gene