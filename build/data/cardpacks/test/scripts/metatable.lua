local card = {}

function card.set_local_var(self)
	self.variable = true
	return self.variable ~= nil
end

return card