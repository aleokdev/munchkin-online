#include "systems/input_binder.hpp"
#include "game.hpp"

#include "input/input.hpp"

#include <iostream>

namespace munchkin {
namespace systems {

InputBinder::InputBinder(Game& g) : game(&g) {}

void InputBinder::tick() {
	math::Vec2D m_pos = (input::get_mouse_pos() + game->camera.offset * math::Vec2D{ (float)game->window_w, -(float)game->window_h } / 2.f - math::Vec2D{ 0, (float)game->window_h }) * math::Vec2D{ 1,-1 };
	for (auto& sprite : game->card_sprites) {
		if (sprite.get_rect().contains(m_pos))
		{
			sprite.is_being_hovered = true;
			if (input::has_mousebutton_been_clicked(input::MouseButton::left) && sprite.get_card_ptr()->location == Card::CardLocation::dungeon_deck)
			{
				game->push_event(FlowEvent{ FlowEvent::EventType::clicked_dungeon_deck });
				break;
			}
		}
		else
			sprite.is_being_hovered = false;
	}
}

}
}