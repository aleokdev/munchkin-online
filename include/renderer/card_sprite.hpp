#ifndef MUNCHKIN_CARD_SPRITE_HPP__
#define MUNCHKIN_CARD_SPRITE_HPP__

#include "api/card.hpp"
#include "util/pos_vec.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "util/pos_vec.hpp"

namespace munchkin {
	class Game;

namespace renderer {

namespace internal {
namespace card_sprite {
	inline static constexpr float movement_slowness = 16;
	inline static constexpr float rotation_slowness = 16;
	inline static constexpr float flip_slowness = 16;
	inline static constexpr float scale_slowness = 16;
	inline static constexpr float texture_scale = 0.2f;
	inline static constexpr float texture_hovered_scale = 0.25f;
	inline static constexpr float texture_width = 454;
	inline static constexpr float texture_height = 704;
}
}

class SpriteRenderer;

class CardSprite {
public:
	CardSprite(Game&, CardPtr);

	void set_target_pos(math::Vec2D target);
	math::Vec2D get_current_pos();
	void instantly_set_pos(math::Vec2D target);
	void calculate_target_from_location();

	void draw(SpriteRenderer&);

	math::Rect2D get_rect();

	bool is_being_hovered = false;

	CardPtr get_card_ptr() { return card; }

private:
	Game* const game;

	math::Vec2D target_pos = { 0,0 };
	math::Vec2D current_pos = { 0,0 };
	float target_rotation = 0;
	float current_rotation = 0;

	math::Vec2D current_size = { internal::card_sprite::texture_width,
							     internal::card_sprite::texture_height };

	float current_scale = internal::card_sprite::texture_scale;

	CardPtr card;
	Card::CardLocation last_card_location = Card::CardLocation::invalid;
	size_t last_cards_in_owner = 0;

	// TODO: Back textures are duplicated! Do asset system and assign all textures that point to the same file to the same ID?
	unsigned int back_texture;

	unsigned int front_texture;
};

}
}

#endif