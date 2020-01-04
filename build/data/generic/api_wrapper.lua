-- api_wrapper.lua
-- Defines API functions pretty much generic to all game rules, such as iter_players
-- and wrappers for the state variable

-- Note: Try to stay away from IDs! They cause confusion, since they start at 0!
-- Use game.iter_players() and other wrappers instead!

function munchkin_state.iter_players(self)
    local i = -1
    return function()
        i = i + 1
        if i == self:get_player_count() then return end
        return self:get_player(i)
    end
end

function munchkin_state.next_player_turn(self)
    self:set_current_player((self:get_current_player() + 1) % self:get_player_count())
    self.current_turn = self.current_turn + 1
end