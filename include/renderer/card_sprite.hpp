#ifndef MUNCHKIN_CARD_SPRITE_HPP__
#define MUNCHKIN_CARD_SPRITE_HPP__

#include "api/card.hpp"
#include "util/pos_vec.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "util/pos_vec.hpp"

namespace munchkin {

namespace renderer {

class SpriteRenderer;

class CardSprite {
public:
	CardSprite(CardPtr);

	void set_target_pos(math::Vec2D target);
	math::Vec2D get_current_pos();
	void instantly_set_pos(math::Vec2D target);
	void calculate_target_from_location();

	void draw(SpriteRenderer&);

	math::Rect2D get_rect();

	bool is_being_hovered = false;

private:
	math::Vec2D target_pos = { 0,0 };
	float target_rotation = 0;
	math::Vec2D current_pos = { 0,0 };
	float current_rotation = 0;

	inline static constexpr float movement_slowness = 16;
	inline static constexpr float rotation_slowness = 16;
	inline static constexpr float flip_slowness = 16;
	inline static constexpr float texture_scale = 0.2f;
	inline static constexpr float texture_width = 454;
	inline static constexpr float texture_height = 704;

	math::Vec2D current_size = { texture_width * texture_scale, texture_height * texture_scale };

	CardPtr card;
	Card::CardLocation last_card_location = Card::CardLocation::invalid;

	// TODO: Back textures are duplicated! Do asset system and assign all textures that point to the same file to the same ID?
	unsigned int dungeon_back_texture;

	unsigned int default_front_texture;
};

}

}

#endif