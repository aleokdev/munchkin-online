game:get_current_player().level = game:get_current_player().level - 1
if game:get_current_player().level < 1 then
	game:get_current_player().level = 1
end