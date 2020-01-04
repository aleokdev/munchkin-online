-- rules.lua
-- Defines specific API functions that determine the rules of a game (Such as when a game is finished, or who has won the game)

function munchkin_state.get_winner(self) 
    for player in self:iter_players() do
        if player.level >= 10 then
            return player
        end
    end
    return nil
end

function munchkin_state.has_ended(self)
    return self:get_winner() ~= nil
end