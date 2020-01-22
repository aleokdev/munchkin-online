.. highlight:: lua

:class:`munchkin_battle` Class
=============================
The :class:`munchkin_battle` class represents a battle between one or more players and one or more monsters.
This class should not be instantiated; Instead, it's normally obtained via the :classmethod:`munchkin_game.current_battle`
variable.

.. note::
   :class:`munchkin_battle` doesn't support player helping yet.

Functions and data
------------------
General information
^^^^^^^^^^^^^^^^^^^
.. data:: treasures_to_draw

   Represents the number of treasures to draw from the treasure deck when the battle ends and is won.

   :vartype: int

.. classmethod:: get_total_player_power()

   Returns the total player-side power (Calculated via the player's level and armor bonuses)

   :rtype: int

.. classmethod:: get_total_monster_power()

   Returns the total monster-side power (Which is the sum of power of every card present in the battle added
   via :classmethod:`add_card`)

   :rtype: int

.. classmethod:: add_card(card)
   
   Adds a card to the battle with 0 power. The card's location is automatically set to `card_location.table_center`.

