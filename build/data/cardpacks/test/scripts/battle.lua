local card = {}

function card.test_battle_1(self)
	return game.current_battle == nil
end

function card.test_battle_2(self)
	game:start_battle()
	return game.current_battle ~= nil
end

function card.test_battle_3(self)
	game:end_current_battle()
	return game.current_battle == nil
end

return card