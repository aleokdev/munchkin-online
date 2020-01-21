.. highlight:: lua

:class:`munchkin_state` Class
=============================
The :class:`munchkin_state` class represents a game state, which includes only the actual game information,
and not systems such as input or rendering. This class should not be instantiated; Instead, the
`game` variable is given, which contains the current game state.

Functions and data
------------------
General information
^^^^^^^^^^^^^^^^^^^
.. data:: last_event

   Represents the last event recorded by the game.

   :vartype: flow_event

   .. warning::
      `last_event` might be replaced by a `coroutine.yield` return value at some point.

.. classmethod:: get_ticks()

   Counts up each time the game sends the `tick` event. Starts at 0.

   :rtype: int

.. data:: stage

   Represents the game's stage. This value is a string, and determines which cards can be played
   at any given moment. It also describes what is currently happening in the game.

   In the default gamerules, this value can be any of the following:
   * `EQUIP_AND_OPEN_DUNGEON`
   * `FIGHT_MONSTER`
   * `DECIDE_NOMONSTER`
   * `GET_TREASURE`
   * `CHARITY`
   * `FLEE_MONSTER`
   * `SUCCESSFUL_FLEE`
   For more information, `here's a diagram explaining the relation between those stages. <https://drive.google.com/file/d/13Kpp6ZPXDKrDfIHnG4Z4bXMH0h5eotn5/view?usp=sharing>`_

   :vartype: str

.. data:: turn_number
   
   This value starts at 0, and increases every time `next_player_turn` is called.

   :vartype: int

Card Borrowing
^^^^^^^^^^^^^^
.. classmethod:: munchkin_state.give_treasure(player)

   Moves one treasure card from the treasure deck to a player's hand. Takes into account the
   :data:`should_borrow_facing_up` variable.

   .. warning::
      `should_borrow_facing_up` will be deprecated at some point and the function will change to
      `give_treasure(player, visibility = card_visibility.front_visible_to_owner)`.

.. classmethod:: munchkin_state.give_dungeon(player)

   Moves one dungeon card from the dungeon deck to a player's hand. Takes into account the
   :data:`should_borrow_facing_up` variable.

   .. warning::
      `should_borrow_facing_up` will be deprecated at some point and the function will change to
      `give_treasure(player, visibility = card_visibility.front_visible_to_owner)`.

.. data:: munchkin_state.should_borrow_facing_up

   Indicates if the next card drawn from a deck will face up or down.

   .. warning::
      `should_borrow_facing_up` will be deprecated at some point due to poor design.

Battles
^^^^^^^
.. classmethod:: munchkin_state.start_battle(player)

.. data:: munchkin_state.current_battle

   The current :class:`munchkin_battle` active in the game, or nil if none exists or the last one has
   ended.

.. classmethod:: munchkin_state.end_current_battle()

Players
^^^^^^^
.. classmethod:: munchkin_state.get_player(id)
    
    Returns a player that has a specific ID. Returns nil if it couldn't be found.

    :rtype: munchkin_player

.. classmethod:: munchkin_state.get_player_count()
    
    Returns the number of players in this game.

    :rtype: int

.. classmethod:: munchkin_state.get_current_player()

    Returns the user that is currently playing.

    :rtype: munchkin_player

.. classmethod:: munchkin_state.set_current_player(id)

    Sets the user that is currently playing via its ID. Does nothing if it couldn't find a player with the
    ID specified.

    :rtype: munchkin_player

.. classmethod:: munchkin_state.next_player_turn()

    Increases `turn_number` by one, and changes the user that is currently playing.

    .. note::
       If you want to use this function, remember to also reset `stage` to its default value so the next user
       starts where every other user does.

Cards
^^^^^
.. classmethod:: munchkin_state.get_visible_cards()
    
    Returns all the cards visible on the table, which includes all the current battle cards, all the cards of
    other players, and the first card of each deck.

    :rtype: munchkin_card_ptr[]

.. classmethod:: munchkin_state.get_dungeon_deck_front()
    
    Returns the front (topmost card) of the dungeon deck.

    :rtype: munchkin_card_ptr

.. classmethod:: munchkin_state.get_dungeon_deck_size()
    
    Returns the size of the dungeon deck.

    :rtype: int

.. classmethod:: munchkin_state.get_dungeon_deck_pop()
    
    Removes the topmost card of the dungeon deck from it.



.. classmethod:: munchkin_state.get_dungeon_discard_deck_front()
    
    Returns the front (topmost card) of the dungeon discard deck.

    :rtype: munchkin_card_ptr

.. classmethod:: munchkin_state.get_dungeon_discard_deck_size()
    
    Returns the size of the dungeon discard deck.

    :rtype: int

.. classmethod:: munchkin_state.get_dungeon_discard_deck_pop()
    
    Removes the topmost card of the dungeon discard deck from it.



.. classmethod:: munchkin_state.get_treasure_deck_front()
    
    Returns the front (topmost card) of the treasure deck.

    :rtype: munchkin_card_ptr

.. classmethod:: munchkin_state.get_treasure_deck_size()
    
    Returns the size of the treasure deck.

    :rtype: int

.. classmethod:: munchkin_state.get_treasure_deck_pop()
    
    Removes the topmost card of the treasure deck from it.



.. classmethod:: munchkin_state.get_treasure_discard_deck_front()
    
    Returns the front (topmost card) of the treasure discard deck.

    :rtype: munchkin_card_ptr

.. classmethod:: munchkin_state.get_treasure_discard_deck_size()
    
    Returns the size of the treasure discard deck.

    :rtype: int

.. classmethod:: munchkin_state.get_treasure_discard_deck_pop()
    
    Removes the topmost card of the treasure discard deck from it.



.. data:: munchkin_state.default_hand_max_cards

    The number of maximum cards a player can have in their hand. This value *should be* set in the `rules.lua`
    file and is only used in the scripting API, not internally. Furthermore, there is no guarantee that
    this value will be set at any given time.

    :vartype: int