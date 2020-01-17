local card = {}

function card.on_play(self) 
    game:get_current_player().level = game:get_current_player().level + 1;
end

return card