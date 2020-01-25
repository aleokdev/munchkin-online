local function wait_for_event(ev)
	repeat
		coroutine.yield()
	until game.last_event.type == ev
end

local function wait_for_ticks(ticks)
	local ticks_before = game:get_ticks()
	repeat
		coroutine.yield()
	until game:get_ticks() >= (ticks_before + ticks)
end

-- Waits for a number or ticks, or until an event has happened.
-- Returns true if waited for all the ticks, false if the event happened first.
local function wait_for_ticks_or_event(ev, ticks)
	local ticks_before = game:get_ticks()
	repeat
		coroutine.yield()
	until game:get_ticks() >= (game:get_ticks() + ticks) or game.last_event.type == ev
	return game:get_ticks() >= (game:get_ticks() + ticks)
end

local function has_value (tab, val)
    for index, value in ipairs(tab) do
        if value == val then
            return true
        end
    end

    return false
end

-- Returns true if the event given is authorized (Is in its play stages and involves a player's card)
local function is_playermove_allowed(event)
	return event.card_involved ~= nil
	and event.card_involved:get():get_location() == card_location.player_hand
	and event.card_involved:get().owner_id == game:get_current_player().id
	and has_value(event.card_involved:get():get_def().play_stages, game.stage)
end

local function stage_equip_stuff()
	repeat wait_for_event(event_type.card_clicked)
	until game.last_event.card_involved:get():get_location() == card_location.dungeon_deck
		
	if game:get_dungeon_deck_size() == 0 then
		print("Out of cards to draw!")
		return -- end the game
	end

	card_ptr = game:get_dungeon_deck_front()
	card_ptr:get():move_to(card_location.table_center, 0)
	card_ptr:get().visibility = card_visibility.front_visible
	on_reveal = card_ptr:get()["on_reveal"]

	if on_reveal ~= nil then
		print("Found on_reveal...")
		on_reveal(card_ptr)
	end
		
	if game.current_battle ~= nil then
		game.stage = "FIGHT_MONSTER"
	else
		wait_for_ticks(120) -- Wait for a while before giving the card to the player
		card_ptr:get():move_to(card_location.player_hand, game:get_current_player().id)
		card_ptr:get().visibility = card_visibility.front_visible_to_owner
		game.stage = "DECIDE_NOMONSTER"
	end
end

local function stage_fight_monster()
	print("stage_fight_monster")
	repeat
		coroutine.yield()
		if game.last_event.type == event_type.card_clicked then
			-- Calculate if the card clicked can be played or not
			if is_playermove_allowed(game.last_event) then
				local c = game.last_event.card_involved:get()
				c["on_play"](c)
			end
		end
	until false -- Replace with <when player wants to end battle>
		
	local ticks_to_wait = 2.6 * 60 -- "When you kill a monster, you must wait a reasonable time, defined as about 2.6 seconds,"

	while wait_for_ticks_or_event(event_type.card_played, ticks_to_wait) do end

	-- People have stopped playing cards, so we can continue.
	-- Check if the user(s) have won the battle.
	if game.current_battle.get_total_player_power() > game.current_battle.get_total_monster_power() then
		-- Players have won, woo!
		game.stage = "GET_TREASURE"
	else
		-- Monsters have won, woo!
		game.stage = "FLEE_MONSTER"
	end
end

local function stage_decide_nomonster()
	while true do
		if game.current_battle ~= nil then
			-- User decided to play a monster of their own
			game.stage = "FIGHT_MONSTER"
		end
		if game.last_event.type == event_type.card_clicked and game.last_event.card_involved:get():get_location() == card_location.dungeon_deck then
			-- User decided to loot the room
			game:give_dungeon(game:get_current_player())

			game.stage = "CHARITY"
			break
		end

		coroutine.yield()
	end
end

local function stage_get_treasure()
	for i=0, game.current_battle.treasures_to_draw do
		wait_for_event(event_type.treasure_card_drawn)
	end

	game.end_current_battle()

	game.stage = "CHARITY"
end

local function stage_charity()
	while #game:get_current_player().hand > game:get_current_player().hand_max_cards do
		wait_for_event(event_type.card_discarded)
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
		for i=1, 4 do
			game:give_dungeon(player)
			wait_for_ticks(10)
		end
		for i=1, 4 do
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