#include "renderer/card_sprite.hpp"
#include "api/state.hpp"
#include <algorithm>
#include "renderer/sprite_renderer.hpp"
#include "renderer/util.hpp"

#include <glad/glad.h>
#include <sdl/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace munchkin {

namespace renderer {

CardSprite::CardSprite(CardPtr _card) : card(_card)
{
    // TODO: Get texture filename from card
    dungeon_back_texture = renderer::load_texture("data/cardpacks/default/textures/dungeon-back.png");
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

void CardSprite::draw(SpriteRenderer& spr)
{
    // Set draw data    
    spr.set_texture(dungeon_back_texture);
    spr.set_position(glm::vec2(current_pos.x, current_pos.y));
    spr.set_scale(glm::vec2(texture_width*texture_scale, texture_height*texture_scale));

	spr.do_draw();
}

}

}