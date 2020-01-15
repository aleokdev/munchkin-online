#include "api/card.hpp"
#include "util/pos_vec.hpp"

namespace munchkin {

struct CardSprite {
	enum class CardPos {
		invalid,
		dungeon_deck,
		dungeon_discard_deck,
		treasure_deck,
		treasure_discard_deck
	};
	CardSprite(CardPtr);

	CardPos pos_enum;
	pos_vec::Vec2D pos;
	CardPtr card;
};

}