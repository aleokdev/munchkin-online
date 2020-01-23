#include "systems/input_binder.hpp"
#include "game.hpp"

#include "input/input.hpp"

#include <iostream>

namespace munchkin {
namespace systems {

InputBinder::InputBinder(Game& g) : game(&g) {}

void InputBinder::tick() {
	// Interacting with cards //

	float frame_time = (float)SDL_GetTicks() / 1000.0f;
	float delta_time = frame_time - last_frame_time;
	last_frame_time = frame_time;

	math::Vec2D m_pos = (input::get_mouse_pos() + game->camera.offset * math::Vec2D{ (float)game->window_w, -(float)game->window_h } / 2.f - math::Vec2D{ 0, (float)game->window_h }) * math::Vec2D{ 1,-1 };
	for (auto& sprite : game->card_sprites) {
		if (sprite.get_rect().contains(m_pos))
		{
			sprite.is_being_hovered = true;
			if (input::has_mousebutton_been_clicked(input::MouseButton::left))
			{
				switch (sprite.get_card_ptr()->get_location())
				{
				case(Card::CardLocation::dungeon_deck):
					game->push_event(FlowEvent{ FlowEvent::EventType::clicked_dungeon_deck });
					break;

				case(Card::CardLocation::player_hand):
				{
					// TODO: Check for current stage, current player and most importantly: PUSH THIS TO ACTIVE_COROUTINES (doesn't work right now for some reason)
					sol::object on_play = sprite.get_card_ptr()->get_data_variable("on_play");
					if (on_play == sol::lua_nil)
						break;
					on_play.as<sol::function>()();
					sprite.get_card_ptr()->move_to(sprite.get_card_ptr()->get_def().category == DeckType::dungeon ? Card::CardLocation::dungeon_discard_deck : Card::CardLocation::treasure_discard_deck);
					
					break;
				}

				default: break;
				}
				
			}
		}
		else
			sprite.is_being_hovered = false;
	}

	// Panning //

	constexpr float pan_speed = 0.1f;

	// only enable panning if left mouse button is clicked
	if (input::is_mousebutton_pressed(input::MouseButton::left)
		& last_mouse_state.button_flagmap & SDL_BUTTON(SDL_BUTTON_LEFT)) {
		math::Vec2D offset = input::get_mouse_pos() - math::Vec2D{ (float)last_mouse_state.x, (float)last_mouse_state.y };

		game->camera.offset.x -= offset.x * pan_speed * delta_time;
		game->camera.offset.y += offset.y * pan_speed * delta_time;
	}

	last_mouse_state = input::get_current_mouse_state();
}

}
}