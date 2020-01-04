local function wait_for_event(ev)
	repeat
		coroutine.yield()
	until game.last_event.name == ev
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
	until game:get_ticks() >= (game:get_ticks() + ticks) or game.last_event.name == ev
	return game:get_ticks() >= (game:get_ticks() + ticks)
end

local function stage_equip_stuff()
	game:open_dungeon()
	wait_for_event("dungeon_opened")
	-- if game.last_card_drawn.script_name == "monster.lua" then -- use later, no support in the api for last_card_drawn yet
	if game.current_battle ~= nil then
		game.stage = "FIGHT_MONSTER"
	else
		game.stage = "DECIDE_NOMONSTER"
	end
end

local function stage_fight_monster()
	-- FOGGY TERRITORY: Still no idea how we're going to implement this. --

	wait_for_event("click_stop_battle_button")

	-- FOGGY TERRITORY END --
		
	local ticks_to_wait = 2.6 * 60 -- "When you kill a monster, you must wait a reasonable time, defined as about 2.6 seconds,"

	::wait_again::
	if not wait_for_ticks_or_event("card_played", ticks_to_wait) then
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

local function stage_get_treasure()
	for i=0, game.current_battle.treasures_to_draw do
		wait_for_event("treasure_card_drawn")
	end

	game.end_current_battle()

	game.stage = "CHARITY"
end

local function stage_charity()
	repeat
		wait_for_event("card_discarded")
	until #game:get_current_player().hand <= game:get_current_player().hand_max_cards
	game.stage = "EQUIP_STUFF_AND_OPEN_DUNGEON"
	game.next_player_turn()
	coroutine.yield()
end

local function main()
	local stages = { EQUIP_STUFF_AND_OPEN_DUNGEON = stage_equip_stuff, FIGHT_MONSTER = stage_fight_monster, GET_TREASURE = stage_get_treasure, CHARITY = stage_charity }
	
	game.stage = "EQUIP_STUFF_AND_OPEN_DUNGEON"
	while true do
		stages[game.stage]()
	end
end

return main