#include "renderer/card_sprite.hpp"
#include "api/state.hpp"
#include <algorithm>

namespace munchkin {

CardSprite::CardSprite(CardPtr _card) : card(_card)
{
	bool is_in_dungeon_deck = false;
	for (auto& card : _card.state->dungeon_deck)
		if (card == _card) {
			is_in_dungeon_deck = true;
			break;
		}
	if (is_in_dungeon_deck) {
		pos_enum = CardPos::dungeon_deck;
		return;
	}

	pos_enum = CardPos::invalid;
}

}