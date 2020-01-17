local card = {}

function card.on_reveal(self):
	game:start_battle()
	game.current_battle:add_card(self)
	game.current_battle:modify_card(self, card.properties.power)
end

function card.bad_stuff(self):
	
end

return card