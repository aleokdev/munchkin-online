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

function game.next_player_turn()
    state:set_current_player((state:get_current_player() + 1) % state:get_player_count())
    state.current_turn = state.current_turn + 1
end

-- Creates a reference to state.last_event since it is a table, and tables are passed by reference.
game.last_event = state.last_event