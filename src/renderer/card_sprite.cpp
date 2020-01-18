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

#include <cmath>
#include "game.hpp"

namespace munchkin {

namespace renderer {

CardSprite::CardSprite(Game& g, CardPtr _card) : game(&g), card(_card)
{
    // TODO: Get texture filename from card
    dungeon_back_texture = renderer::load_texture("data/cardpacks/default/textures/dungeon-back.png");
    default_front_texture = renderer::load_texture("data/cardpacks/default/textures/dungeon-front.png");
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

void CardSprite::calculate_target_from_location()
{
    switch (card->location)
    {
    case munchkin::Card::CardLocation::invalid:
        target_pos = { 0, 100 }; // test value
        break;

    case munchkin::Card::CardLocation::dungeon_deck:
        target_pos = { texture_width * texture_scale / 3.f * 2.f, 0 };
        break;

    case munchkin::Card::CardLocation::dungeon_discard_deck:
        target_rotation = M_PI;
        target_pos = { texture_width * texture_scale / 3.f * 2.f, texture_height * texture_scale };
        break;

    case munchkin::Card::CardLocation::treasure_deck:
        target_pos = { -texture_width * texture_scale / 3.f * 2.f, 0 };
        break;

    case munchkin::Card::CardLocation::treasure_discard_deck:
        target_pos = { -texture_width * texture_scale / 3.f * 2.f, texture_height * texture_scale };
        break;

    case munchkin::Card::CardLocation::player_hand:
    {
        Player& card_owner = card.state->players[card->owner_id];
        float player_angle = ((float)card_owner.id) / ((float)card.state->player_count) * 2.f * M_PI - M_PI / 2.f;
        // TODO: Don't assume table radius
        // this isn't actually the table radius, it's just the radius of an imaginary circurference where all the cards are placed
        constexpr float table_radius = 750;
        math::Vec2D player_pos{ table_radius * std::cos(player_angle), table_radius * std::sin(player_angle) };
        target_pos = player_pos;
        target_rotation = player_angle + M_PI / 2.f;
        break;
    }

    case munchkin::Card::CardLocation::player_equipped:
    {
        Player& card_owner = card.state->players[card->owner_id];
        float player_angle = ((float)card_owner.id) / ((float)card.state->player_count) * 2.f * M_PI - M_PI / 2.f;
        // TODO: Don't assume table radius
        // this isn't actually the table radius, it's just the radius of an imaginary circurference where all the cards are placed
        constexpr float table_radius = 450;
        math::Vec2D player_pos{ table_radius * std::cos(player_angle), table_radius * std::sin(player_angle) };
        target_pos = player_pos;
        target_rotation = player_angle + M_PI / 2.f;
        break;
    }

    case munchkin::Card::CardLocation::battle:
        target_pos = { 0, texture_height * texture_scale / 3.f * 2.f };
        break;

    default:
        break;
    }
}

void CardSprite::draw(SpriteRenderer& spr)
{
    if (last_card_location != card->location) {
        calculate_target_from_location();
        last_card_location = card->location;
    }

    current_pos += (target_pos - current_pos) / movement_slowness;
    current_rotation += (target_rotation - current_rotation) / rotation_slowness;
    const bool is_card_visible = card->visibility == Card::CardVisibility::front_visible || (card->visibility == Card::CardVisibility::front_visible_to_owner && card->owner_id == game->local_player_id);
    current_size += math::vectors::x_axis * ((is_card_visible ? (-texture_width * texture_scale) : (texture_width * texture_scale)) - current_size.x) / flip_slowness;

    // Set draw data
    if (current_size.x > 0)
        spr.set_texture(dungeon_back_texture);
    else
        spr.set_texture(default_front_texture);
    spr.set_position(glm::vec2(current_pos.x, current_pos.y));
    spr.set_scale(glm::vec2(std::abs(current_size.x), current_size.y));
    spr.set_rotation(current_rotation);
    if (is_being_hovered)
        spr.set_color(1.1f, 1.1f, 1.1f, 1);

	spr.do_draw();
}

math::Rect2D CardSprite::get_rect()
{
    return math::Rect2D{ current_pos - current_size.abs()/2.f, current_size.abs() };
}

}

}