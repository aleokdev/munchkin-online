#include "api/card.hpp"
#include "util/pos_vec.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace munchkin {

namespace renderer {

class SpriteRenderer;

class CardSprite {
public:
	CardSprite(CardPtr);

	void set_target_pos(math::Vec2D target);
	math::Vec2D get_current_pos();
	void instantly_set_pos(math::Vec2D target);

	void draw(SpriteRenderer&);

private:
	math::Vec2D target_pos = { 0,0 };
	math::Vec2D current_pos = { 0,0 };
	const int movement_slowness = 8;
	inline static constexpr float texture_scale = 0.1f;
	inline static constexpr float texture_width = 454;
	inline static constexpr float texture_height = 704;
	CardPtr card;

	// TODO: Textures are duplicated! Do asset system and assign all textures that point to the same file to the same ID?
	unsigned int dungeon_back_texture;
};

}

}