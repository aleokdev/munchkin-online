-- api_wrapper.lua
-- Defines API functions pretty much generic to all game rules, such as iter_players
-- and wrappers for the state variable

game = {}

-- Note: Try to stay away from IDs! They cause confusion, since they start at 0!
-- Use game.iter_players() and other wrappers instead!

function game.iter_players()
    local i = -1
    return function()
        i = i + 1
        if i == state:get_player_count() then return end
        return state:get_player(i)
    end
end
