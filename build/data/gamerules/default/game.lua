game = {}

function game.iter_players()
    local i = -1
    return function()
        i = i + 1
        if i == state:get_player_count() then return end
        return state:get_player(i)
    end
end

function game.winner() 
    for player in game.iter_players() do
        if player.level >= 10 then
            return player
        end
    end
    return nil
end

function game.has_ended()
    return game.winner() ~= nil
end