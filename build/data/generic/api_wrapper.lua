-- api_wrapper.lua
-- Defines API functions pretty much generic to all game rules, such as iter_players
-- and wrappers for the state variable

-- Note: Try to stay away from IDs! They cause confusion, since they start at 0!
-- Use game.iter_players() and other wrappers instead!

-- munchkin_state

function munchkin_state.iter_players(self)
    local i = -1
    return function()
        i = i + 1
        if i == self:get_player_count() then return end
        return self:get_player(i)
    end
end

function munchkin_state.next_player_turn(self)
    self:set_current_player((self:get_current_player().id + 1) % self:get_player_count())
    self.turn_number = self.turn_number + 1
end

-- input

selection = {}

function selection.choose_card(filter)
    local selection_cards = {}

    if filter == nil then
        for card in pairs(game.all_cards) do
            selection_cards:insert(card)
        end
    else
        for card in pairs(game.all_cards) do
            if filter(card) then
                selection_cards:insert(card)
            end
        end
    end

    repeat
        coroutine.yield()
    until game.last_event.type == event_type.card_clicked

    return game.last_event.card_involved
end