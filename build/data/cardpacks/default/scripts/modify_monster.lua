local card = {}

function card.on_play(self) 
    selection = selection.choose_filtered(function(card) return game.current_battle[card] ~= nil end)
    if selection ~= nil then
        game.current_battle.modify_card(selection, 5)
        self.target_monster = selection
    end
end

return card