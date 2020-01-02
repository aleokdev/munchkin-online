function end_game(state) 
    state.game_ended = true;
end

function on_play(state, player)
    player.level = 6;
end