local card = {}

function card.on_play(self) 
    game:get_current_player().level = game:get_current_player().level + 1;
    self:move_to(card_location.treasure_discard_deck, 0)
end

return card