local card = {}

function card.check_if_current_battle_is_nil(self)
	return game.current_battle == nil
end

function card.start_battle(self)
	game:start_battle()
	return game.current_battle ~= nil
end

function card.end_battle(self)
	game:end_current_battle()
	return game.current_battle == nil
end

return card