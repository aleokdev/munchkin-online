local card = {}

function card.on_play(self) 
    game.current_player.level = game.current_player.level + 1;
end

return card