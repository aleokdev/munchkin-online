local card = {}

function card.on_reveal(self)
	print("monster.lua on_reveal")
	print("self = " .. tostring(self))
	print("card = " .. tostring(card))
	for k,v in pairs(card) do
		print("\t"..tostring(k).." = "..tostring(v))
	end
	for k,v in pairs(card.properties) do
		print("\t"..tostring(k).." = "..tostring(v))
	end
	print("power = " .. tostring(card.properties.power))
	game:start_battle()
	game.current_battle:add_card(self)
	game.current_battle:modify_card(self, card.properties.power)
end

function card.battle_end(self)
	
end

print("monster.lua loaded")
return card