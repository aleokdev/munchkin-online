game = {}

-- Game API --

-- Try to stay away from IDs! They cause confusion, since they start at 0!
-- Use game.iter_players() and other wrappers instead!

function game.iter_players()
    local i = -1
    return function()
        i = i + 1
        if i == state:get_player_count() then return end
        return state:get_player(i)
    end
end

-- Actual Game Rules --

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