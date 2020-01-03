local card = {}

function card.on_play(self) 
    local function selection_filter(card)
        local is_present_in_battle = game.current_battle[card] ~= nil
        local is_monster = card.attributes["is_monster"]
        return is_present_in_battle and is_monster
    end

    selection = selection.choose_filtered(selection_filter)
    if selection ~= nil then
        game.current_battle.add_card(self)
        game.current_battle.modify_card(selection, 5)
        self.target_monster = selection
    end
end

function card.on_battle_end(self)
    self.target_monster = nil
end

function card.on_anhilate(self)
    if self.target_monster ~= nil then
    end
end

return card