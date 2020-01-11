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

function munchkin_state.init_rules(self)
    self.default_hand_max_cards = 5
end

function munchkin_state.next_player_turn(self)
    self:set_current_player((self:get_current_player().id+1)%self:get_player_count())
end