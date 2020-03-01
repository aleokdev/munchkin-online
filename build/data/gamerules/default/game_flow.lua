local function has_value(tab, val)
    for index, value in ipairs(tab) do
        if value == val then
            return true
        end
    end

    return false
end

local function discard(card)
    card.visibility = card_visibility.front_visible
    if card:get_def().category == munchkin_deck_type.dungeon then
        card:move_to(card_location.dungeon_discard_deck, 0)
    else
        card:move_to(card_location.treasure_discard_deck, 0)
    end
end

-- Returns true if the event given is authorized (Is in its play stages and involves a player's card)
local function is_playermove_allowed(event)
    return event.card_involved ~= nil
            and event.card_involved:get_location() == card_location.player_hand
            and event.card_involved.owner_id == event.player_id_involved
            and event.card_involved.owner_id == game:get_current_player().id
            and has_value(event.card_involved:get_def().play_stages, game.stage)
end

------------
-- STAGES --
------------
local function stage_equip_stuff()
    repeat wait_for_event(event_type.card_clicked)
        if is_playermove_allowed(game.last_event) then
            game.last_event.card_involved["on_play"](game.last_event.card_involved)
        end
    until game.last_event.card_involved:get_location() == card_location.dungeon_deck

    if game:get_dungeon_deck_size() == 0 then
        print("Out of cards to draw!")
        return -- end the game
    end

    local card = game:get_dungeon_deck_front()
    card:move_to(card_location.table_center, 0)
    card.visibility = card_visibility.front_visible
    local on_reveal = card["on_reveal"]

    if on_reveal ~= nil then
        print("Found on_reveal...")
        on_reveal(card)
    end

    if game.current_battle ~= nil then
        game.stage = "FIGHT_MONSTER"
    else
        wait_for_ticks(120) -- Wait for a while before giving the card to the player
        card:move_to(card_location.player_hand, game:get_current_player().id)
        card.visibility = card_visibility.front_visible_to_owner
        game.stage = "DECIDE_NOMONSTER"
    end
end

local function stage_fight_monster()
    print("stage_fight_monster")
    while true do
        coroutine.yield()
        if game.last_event.type == event_type.card_clicked then
            print("clicked!")
            -- Calculate if the card clicked can be played or not
            if is_playermove_allowed(game.last_event) then
                local c = game.last_event.card_involved
                c["on_play"](c)
                if game.current_battle:get_total_player_power() > game.current_battle:get_total_monster_power() then
                    break
                end
            else
                print("but the playermove wasn't allowed...")
            end
        end
    end
    print("Ok, monster should be defeated by now!")

    local ticks_to_wait = 2.6 * 60 -- "When you kill a monster, you must wait a reasonable time, defined as about 2.6 seconds,"
    local ticks_waited = 0
    while ticks_waited < ticks_to_wait do
        coroutine.yield()

        if game.last_event.type == event_type.tick then
            ticks_waited = ticks_waited + 1
        elseif game.last_event.type == event_type.card_clicked and is_playermove_allowed(game.last_event) then
            -- Someone used a card, apply it and wait another 2.6 seconds
            event_type.card_involved["on_play"](event_type.card_involved)
            ticks_waited = 0
        end
    end

    -- People have stopped playing cards, so we can continue.
    -- Check if the user(s) have won the battle.
    if game.current_battle:get_total_player_power() > game.current_battle:get_total_monster_power() then
        -- Players have won, woo!
        local cur_player_name = game:get_current_player().name
        local cur_monster_name = game.current_battle.source_card:get_def().name
        logger:log(cur_player_name .. " has defeated " .. cur_monster_name .. "!")
        game.stage = "GET_TREASURE"
    else
        -- Monsters have won, woo!
        local cur_player_name = game:get_current_player().name
        local cur_monster_name = game.current_battle.source_card:get_def().name
        logger:log(cur_player_name .. " has been defeated by " .. cur_monster_name .. "!")
        game.stage = "FLEE_MONSTER"
    end
end

local function stage_decide_nomonster()
    while true do
        if game.current_battle ~= nil then
            -- User decided to play a monster of their own
            game.stage = "FIGHT_MONSTER"
        end
        if game.last_event.type == event_type.card_clicked and game.last_event.card_involved:get_location() == card_location.dungeon_deck then
            -- User decided to loot the room
            game:give_dungeon(game:get_current_player())

            game.stage = "CHARITY"
            break
        end

        coroutine.yield()
    end
end

local function stage_get_treasure()
    for i = 1, game.current_battle.treasures_to_draw do
        game:give_treasure(game:get_current_player())
        wait_for_ticks(10)
    end

    -- Discard all cards on battle
    for k, card in pairs(game.current_battle:get_cards_played()) do
        print(card.id)
        discard(card)
    end
    -- End the battle
    game:end_current_battle()

    game.stage = "CHARITY"
end

local function stage_charity()
    while #game:get_current_player().hand > game:get_current_player().hand_max_cards do
        wait_for_event(event_type.card_clicked)
        if game.last_event.card_involved:get_location() == card_location.player_hand and
                game.last_event.player_id_involved == game:get_current_player().id and
                game.last_event.card_involved.owner_id == game:get_current_player().id then
            discard(game.last_event.card_involved)
        end
    end
    game.stage = "EQUIP_STUFF_AND_OPEN_DUNGEON"
    game:next_player_turn()
    coroutine.yield()
end

local function main()
    local stages = { EQUIP_STUFF_AND_OPEN_DUNGEON = stage_equip_stuff, FIGHT_MONSTER = stage_fight_monster, GET_TREASURE = stage_get_treasure, CHARITY = stage_charity, DECIDE_NOMONSTER = stage_decide_nomonster }

    game.stage = "EQUIP_STUFF_AND_OPEN_DUNGEON"
    wait_for_event(event_type.tick) -- Wait for the game to load cardpacks in

    -- Give 4 cards of each type to the players
    for player in game:iter_players() do
        for i = 1, 4 do
            game:give_dungeon(player)
            wait_for_ticks(10)
        end
        for i = 1, 4 do
            game:give_treasure(player)
            wait_for_ticks(10)
        end
    end

    while true do
        print("Stage: " .. game.stage)
        stages[game.stage]()
    end
end

return main