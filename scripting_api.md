## Scripting API
Stuff that `game` must define (Not official):
- `game.last_event`: FlowEvent -> The last event recorded by the game (readonly)
- `game.get_ticks()`: int -> The game_flow ticker (counts up after game_flow.lua is executed)
- `game.stage`: string -> The game stage (read/write)

- `game.give_treasure(player)`: void -> Gives 1 card of treasure from the deck to the given player, directly, without calling on_reveal first.
- `game.give_dungeon(player)`: void -> Gives 1 card of dungeon from the deck to the given player, directly, without calling on_reveal first.
- `game.open_dungeon()`: void -> Calls on_reveal for the first card of the deck and then discards it, or, if on_reveal is not defined for that card, gives the card to the current player.
- `game.should_borrow_facing_up`: bool -> Should the next card be borrowed facing up or down? (read/write)

- `game.start_battle()
- `game.get_current_battle()`: Battle -> Returns the current battle happening, nil if there isn't any
- `game.end_current_battle()`: void -> Ends the current battle (Or does nothing if the current battle is nil). Does NOT give treasure, use `game.give_treasure(player)` for that.

- `game.get_current_player()`: Player -> Returns the user that is currently playing
- `game.next_player_turn()`: void -> Makes current_player be the next player in the game

Stuff that `Battle` must define (Not official):
- `treasures_to_draw`: int -> The number of treasures to draw from this battle, in total. (read/write)

- `get_total_player_power()`: int -> Returns the calculated total power for the players in this battle
- `get_total_monster_power()`: int -> Returns the calculated total power for the monsters in this battle

- `add_card(card)`: void -> Adds a card to the ones being played in the battle. All cards added will be discarded when the battle ends.
- `remove_card(card)`: void -> Removes a card being played in the battle (Added via `add_card`) from the battle.
- `get_cards_played()`: Card[] -> Returns the cards played in this battle (Added via `add_card`)

- `modify_card(card)`: void -> Adds power to an specified card. That card must have been added to the battle via `add_card`. All cards with power over 0 are considered monsters, so don't modify the power of cards that aren't monsters.
- `get_card_power(card)`: int -> Returns the power of an specified card present in this battle. Returns nil if the card inputted hasn't been added to the battle via `add_card`.

Stuff that `FlowEvent` must define:
- `name`: string -> The name of the event. Can be:
```
dungeon_opened: Called in game.open_dungeon().
tick: Called every tick.
click_stop_battle_button: Called in battles, when the "Stop" button has been pressed (When the user wants to end the battle, basically)
treasure_card_drawn: Called when a treasure card is drawn.
dungeon_card_drawn: Called when a dungeon card is drawn.
card_discarded: Called when a card is discarded or given to a player of lower level (Charity)
```

Stuff that `Player` must define:
- `id`: int -> The internal ID of the player. Avoid using it, as it causes confusion.
- `level`: int -> The level of the player.
- `get_hand()`: Card[] -> The cards that the player has in their hand.
- `hand_max_cards`: int -> The maximum number of cards that this player can have in their hand.