-- Default (extremely simple) AI
-- Tailored to work with the default game_flow!

local ai = {}

function ai.on_turn(self)
	wait_for_ticks(60)

	while game:get_current_player().id == self.player.id do
		if game.state == "EQUIP_STUFF_AND_OPEN_DUNGEON" then
			game:push_event(flow_event.card_clicked, game:get_dungeon_deck_front(), self.player.id)
			
			wait_for_ticks(1) -- Wait for the game_flow to process the card_clicked event
		else if game.state == "FIGHT_MONSTER" then
			-- Try throwing any random card
			game:push_event(flow_event.card_clicked, self.player.hand[math.random(1, #self.player.hand)], self.player.id)

			if game:get_current_battle():get_total_player_power() > game:get_current_battle():get_total_monster_power() then
				-- Battle is won, wait for cards to be discarded
				wait_for_ticks(2.6 * 60) -- "When you kill a monster, you must wait a reasonable time, defined as about 2.6 seconds,"
			else
				wait_for_ticks(30) -- Wait for a while before sending another card
			end
		else if game.state == "CHARITY" then
			-- Discard any random cards until the charity stage ends
			game:push_event(flow_event.card_clicked, self.player.hand[math.random(1, #self.player.hand)], self.player.id)
			
			wait_for_ticks(30) -- Wait for a while before sending another card
		end
	end
end

return ai