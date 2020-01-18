local function main()
	local function wait_for_event(ev)
		repeat
			coroutine.yield()
		until game.last_event.type == ev
	end

	local function wait_for_ticks(ticks)
		local ticks_before = game:get_ticks()
		repeat
			coroutine.yield()
		until game:get_ticks() >= (game:get_ticks() + ticks)
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

	local function stage_equip_stuff()
		wait_for_event(event_type.clicked_dungeon_deck)
		game:open_dungeon() -- will add card.on_reveal to active coroutines if it exists
		wait_for_event(event_type.tick)
		wait_for_event(event_type.tick) -- Wait for card.on_reveal if it exists
		print("finished waiting for card.on_reveal")

		if game.current_battle ~= nil then
			game.stage = "FIGHT_MONSTER"
		else
			game.stage = "DECIDE_NOMONSTER"
		end
	end

	local function stage_fight_monster()
		wait_for_event(event_type.clicked_stop_battle_button)
		
		local ticks_to_wait = 2.6 * 60 -- "When you kill a monster, you must wait a reasonable time, defined as about 2.6 seconds,"

		::wait_again::
		if not wait_for_ticks_or_event(event_type.card_played, ticks_to_wait) then
			-- Some player added another card to the monster or did something, wait again.
			goto wait_again
		end

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
			if game.last_event.type == event_type.clicked_dungeon_deck then
				-- User decided to loot the room
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

	local stages = { EQUIP_STUFF_AND_OPEN_DUNGEON = stage_equip_stuff, FIGHT_MONSTER = stage_fight_monster, GET_TREASURE = stage_get_treasure, CHARITY = stage_charity, DECIDE_NOMONSTER = stage_decide_nomonster }
	
	game.stage = "EQUIP_STUFF_AND_OPEN_DUNGEON"
	wait_for_event(event_type.tick) -- Wait for the game to load cardpacks in
	while true do
		stages[game.stage]()
	end
end

return main