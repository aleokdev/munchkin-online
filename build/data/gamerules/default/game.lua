local game = {}

function game.has_game_ended(state) 
    for i = 1, state:get_player_count() do
        player = state:get_player(i - 1)
        if (player.level >=) 10 then
            return player
        end
    end
    return nil
end

return game