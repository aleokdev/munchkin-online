-- #EDITOR_PROPERTIES
-- #PROPERTY bonus
-- #PROPERTY type
-- #PROPERTY is_big

-- Type can be either: none | headgear | footgear | armor | onehand | twohands

local card = {}

function card:on_play()
    local owner = game:get_current_player()

    if self.properties.is_big then
        for _, equipped_card in owner.equipped do
            if equipped_card.properties.is_big then
                return -- Do not allow equipping more than one big card
            end
        end
    end

    local hands_to_use
    if self.properties.type == "onehand" then hands_to_use = 1
    elseif self.properties.type == "twohands" then hands_to_use = 2 end
    if hands_to_use then
        local owner_free_hands = 2 -- A human normally has two hands, right?
        for _, equipped_card in owner.equipped do
            if self.properties.type == "onehand" then
                owner_free_hands = owner_free_hands - 1
            elseif self.properties.type == "twohands" then
                owner_free_hands = owner_free_hands - 2
            end
        end

        if hands_to_use < owner_free_hands then
            -- Do not allow equipping something that occupies more hands than you have
            return
        end
    elseif self.properties.type ~= "none" then
        for _, equipped_card in owner.equipped do
            if equipped_card.properties.type == self.properties.type then
                -- Do not allow equipping two cards of the same type
                return
            end
        end
    end

    -- All checks successful, the player may equip this card!
    self:move_to(card_location.player_equipped, owner.id)
end

return card