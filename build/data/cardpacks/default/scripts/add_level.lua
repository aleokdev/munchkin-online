local card = {}

-- TODO: Remove state and player args, use game and game.current_player instead
function card.on_play(state, player) 
    player.level = player.level + 1;
end

-- use return because i ain't filling up the global table
return card