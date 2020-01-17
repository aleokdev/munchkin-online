-- Input: levels (Levels to change)

game:get_current_player().level = levels
if game:get_current_player().level < 1 then
	game:get_current_player().level = 1
end