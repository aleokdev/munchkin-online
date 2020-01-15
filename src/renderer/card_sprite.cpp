#include "renderer/card_sprite.hpp"
#include "api/state.hpp"
#include <algorithm>

namespace munchkin {

namespace renderer {

CardSprite::CardSprite(CardPtr _card) : card(_card)
{
}

void CardSprite::set_target_pos(math::Vec2D target)
{
	target_pos = target;
}

math::Vec2D CardSprite::get_current_pos()
{
	return current_pos;
}

void CardSprite::instantly_set_pos(math::Vec2D target)
{
	target_pos = current_pos = target;
}

}

}