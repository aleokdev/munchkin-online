#include "systems/input_binder.hpp"
#include "game.hpp"

#include "input/input.hpp"

#include <algorithm>
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
		Card& sprite_card = *sprite.get_card_ptr();
		if (sprite.get_rect().contains(m_pos))
		{
			sprite.is_being_hovered = true;
			if (input::has_mousebutton_been_clicked(input::MouseButton::left))
			{
				switch (sprite_card.get_location())
				{
				case(Card::CardLocation::dungeon_deck):
					game->push_event(FlowEvent{ FlowEvent::EventType::card_clicked, sprite_card, game->local_player_id });
					break;

				case(Card::CardLocation::player_hand):
				{
					// Do not allow interacting with card if it is not owned by the local player or it is not on an allowed play stage
					if (game->local_player_id != sprite_card.owner_id ||
						std::find(sprite_card.get_def().play_stages.begin(), sprite_card.get_def().play_stages.end(), game->state.get_game_stage()) == sprite_card.get_def().play_stages.end())
						break;

					// TODO: Check for current stage, current player and most importantly: PUSH THIS TO ACTIVE_COROUTINES (doesn't work right now for some reason)
					sol::object on_play = sprite_card.get_data_variable("on_play");
					if (on_play == sol::lua_nil)
						break;
					on_play.as<sol::function>()();
					sprite_card.move_to(sprite_card.get_def().category == DeckType::dungeon ? Card::CardLocation::dungeon_discard_deck : Card::CardLocation::treasure_discard_deck);
					
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