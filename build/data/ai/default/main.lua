-- Default (extremely simple) AI
-- Tailored to work with the default game_flow!

local ai = {}

function ai.on_turn(self)
	local function wait_for_ticks_or_turn(ticks)
		local ticks_before = game:get_ticks()
		repeat
			if game:get_current_player().id ~= self.id then break end
			coroutine.yield()
		until game:get_ticks() >= (ticks_before + ticks)
	end

	print("AI start!")
	wait_for_ticks(60)

	while game:get_current_player().id == self.id do
		if game.stage == "EQUIP_STUFF_AND_OPEN_DUNGEON" then
			game:push_event(event_type.card_clicked, game:get_dungeon_deck_front(), self)
			
			wait_for_ticks_or_turn(1) -- Wait for the game_flow to process the card_clicked event
		elseif game.stage == "FIGHT_MONSTER" then
			-- Try throwing any random card until the stage changes
			-- Simulate clicking a random hand card, then click the battle monster
			-- @todo: [AI] This won't work with multiple monsters in one battle
			game:push_event(event_type.card_clicked, self.hand[math.random(1, #self.hand)], self)
			game:push_event(event_type.card_clicked, game.current_battle.source_card, self)

			wait_for_ticks_or_turn(1) -- Wait for game_flow to process the event

			if game.current_battle:get_total_player_power() > game.current_battle:get_total_monster_power() then
				-- Battle is won, wait for cards to be discarded
				wait_for_ticks_or_turn(2.6 * 60) -- "When you kill a monster, you must wait a reasonable time, defined as about 2.6 seconds,"
			else
				wait_for_ticks_or_turn(30) -- Wait for a while before sending another card
			end
		elseif game.stage == "CHARITY" then
			-- Discard any random cards until the charity stage ends
			game:push_event(event_type.card_clicked, self.hand[math.random(1, #self.hand)], self)
			
			wait_for_ticks_or_turn(30) -- Wait for a while before sending another card
		elseif game.stage == "DECIDE_NOMONSTER" then
			-- Loot the room!
			game:push_event(event_type.card_clicked, game:get_dungeon_deck_front(), self)
			wait_for_ticks_or_turn(60)
		else
			-- Where are we??!
			print("[AI] Unknown game state: '" .. tostring(game.stage) .. "'. Killing coroutine.")
			return
		end
	end

	print("Done!")
	return
end

return ai