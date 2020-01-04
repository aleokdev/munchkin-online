local card = {}

function card.on_play(self) 
    local function selection_filter(card)
        local is_present_in_battle = game.current_battle.get_cards_played()[card] ~= nil
        if not is_present_in_battle then return false end

        local is_monster = game.current_battle.get_card_power(card) > 0
        return is_monster
    end

    selection = selection.choose_filtered(selection_filter)
    if selection ~= nil then
        game.current_battle.add_card(self)
        game.current_battle.modify_card(selection, card.properties.power_to_add)
        self.target_monster = selection
    end
end

function card.on_battle_end(self)
    -- Reset the target_monster so the card doesn't modify it on anhilate
    -- Also because it's good practice to reset the state once the card has finished doing its job
    self.target_monster = nil
end

function card.on_anhilate(self)
    if self.target_monster ~= nil then
        self.target_monster.modify_card(selection, -card.properties.power_to_add)
    end
end

return card