-- rules.lua
-- Defines specific API functions that determine the rules of a game (Such as when a game is finished, or who has won the game)

function munchkin_game:get_winner()
    for player in self:iter_players() do
        if player.level >= 10 then
            return player
        end
    end
    return nil
end

function munchkin_game:has_ended()
    return self:get_winner() ~= nil
end

function munchkin_game:init_rules()
    self.default_hand_max_cards = 5
end