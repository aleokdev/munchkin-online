#include "systems/input_binder.hpp"
#include "game.hpp"

#include "input/input.hpp"

#include <iostream>

namespace munchkin {
namespace systems {

InputBinder::InputBinder(Game& g) : game(&g) {}

void InputBinder::tick() {
	math::Vec2D m_pos = input::get_mouse_pos() + game->camera.offset * math::Vec2D{ (float)game->window_w, -(float)game->window_h } /2.f - math::Vec2D{ 0, (float)game->window_h };
	std::cout << m_pos.x << " " << m_pos.y << std::endl;
	for (auto& sprite : game->card_sprites) {
		sprite.is_being_hovered = sprite.get_rect().contains(m_pos);
	}
}

}
}