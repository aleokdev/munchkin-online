#include "renderer/card_sprite.hpp"
#include "api/state.hpp"
#include "game.hpp"
#include "renderer/sprite_renderer.hpp"
#include "renderer/util.hpp"

#include <algorithm>
#include <cmath>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <sdl/SDL.h>
#include <filesystem>

using namespace munchkin::renderer::internal::card_sprite;

namespace fs = std::filesystem;

namespace munchkin {
namespace renderer {

CardSprite::CardSprite(Game& g, CardPtr _card) : game(&g), card(_card) {
    auto& texture_manager = assets::get_manager<Texture>();
    back_texture_handle = texture_manager.load_asset(card->get_def().back_texture_path, {card->get_def().back_texture_path});
    assets::loaders::LoadParams<Texture> params;
    params.path = fs::path(card->get_def().front_texture_path);
    front_texture_handle = texture_manager.load_asset(card->get_def().front_texture_path, params);
}

void CardSprite::set_target_pos(math::Vec2D target) { target_pos = target; }

math::Vec2D CardSprite::get_current_pos() { return current_pos; }

void CardSprite::instantly_set_pos(math::Vec2D target) { target_pos = current_pos = target; }

void CardSprite::calculate_target_from_location() {
    constexpr float space_between_cards = texture_width * texture_scale / 2.f + 35.f;

    switch (card->get_location()) {
        case munchkin::Card::CardLocation::invalid:
            target_pos = {0, 100}; // test value
            break;

        case munchkin::Card::CardLocation::dungeon_deck:
            target_pos = {texture_width * texture_scale / 3.f * 2.f, 0};
            break;

        case munchkin::Card::CardLocation::dungeon_discard_deck:
            target_rotation = M_PI;
            target_pos = {texture_width * texture_scale / 3.f * 2.f,
                          texture_height * texture_scale};
            break;

        case munchkin::Card::CardLocation::treasure_deck:
            target_pos = {-texture_width * texture_scale / 3.f * 2.f, 0};
            break;

        case munchkin::Card::CardLocation::treasure_discard_deck:
            target_pos = {-texture_width * texture_scale / 3.f * 2.f,
                          texture_height * texture_scale};
            break;

        case munchkin::Card::CardLocation::player_hand: {
            Player& card_owner = card.state->players[card->owner_id];
            const int hand_size = card_owner.hand.size();
            int hand_index = 0;
            for (auto& card_ptr : card_owner.hand) {
                if (card_ptr == card)
                    break;
                hand_index++;
            }

            float player_angle =
                ((float)card_owner.id) / ((float)card.state->player_count) * 2.f * M_PI -
                M_PI / 2.f;
            // radius of an imaginary circurference where all the player hand cards are placed
//            constexpr float distance = renderer::table_radius + 50; // unused variable
            math::Vec2D player_pos{table_radius * std::cos(player_angle),
                                   table_radius * std::sin(player_angle)};

            math::Vec2D move_axis = math::Vec2D{std::cos(player_angle - (float)M_PI / 2.f),
                                                std::sin(player_angle - (float)M_PI / 2.f)};
            math::Vec2D card_pos = math::Vec2D::lerp(
                player_pos - move_axis * hand_size * space_between_cards / 2.f,
                player_pos + move_axis * hand_size * space_between_cards / 2.f,
                (hand_size == 1) ? .5f : (float)hand_index / (float)(hand_size - 1));
            target_pos = card_pos;
            target_rotation = player_angle + M_PI / 2.f;
            break;
        }

        case munchkin::Card::CardLocation::player_equipped: {
            Player& card_owner = card.state->players[card->owner_id];
            float player_angle =
                ((float)card_owner.id) / ((float)card.state->player_count) * 2.f * M_PI -
                M_PI / 2.f;
            // radius of an imaginary circurference where all the equipped cards are placed
            constexpr float distance = renderer::table_radius / 3.f * 2.f;
            math::Vec2D player_pos{distance * std::cos(player_angle),
                                   distance * std::sin(player_angle)};
            target_pos = player_pos;
            target_rotation = player_angle + M_PI / 2.f;
            break;
        }

        case munchkin::Card::CardLocation::table_center:
            if (card->is_on_current_battle()) {
                const int battle_cards = game->state.current_battle->played_cards.size();
                int card_index = 0;
                for (auto& [k, v] : game->state.current_battle->played_cards) {
                    if (k == card)
                        break;
                    card_index++;
                }

                math::Vec2D base_pos{0, -texture_height * texture_scale * 1.6f};
                target_pos = math::Vec2D::lerp(
                    base_pos + math::vectors::left * space_between_cards * battle_cards / 2.f,
                    base_pos + math::vectors::right * space_between_cards * battle_cards / 2.f,
                    (battle_cards == 1) ? .5f : ((float)card_index / (float)(battle_cards - 1)));
            } else {
                target_pos = {0, -texture_height * texture_scale * 1.6f};
            }
            break;

        default: break;
    }
}

void CardSprite::draw(SpriteRenderer& spr) {
    if (last_card_location != card->get_location() ||
        card.state->players[card->owner_id].hand.size() != last_cards_in_owner) {
        calculate_target_from_location();
        last_card_location = card->get_location();
        last_cards_in_owner = card.state->players[card->owner_id].hand.size();
    }

    bool render_darker = false;
    if (card->is_being_owned_by_player() &&
        card.state->get_game_stage() != card.state->get_last_game_stage()) {
        render_darker =
            card->owner_id == game->local_player_id &&
            std::find(card->get_def().play_stages.begin(), card->get_def().play_stages.end(),
                      card.state->get_game_stage()) == card->get_def().play_stages.end();
    }

    current_pos += (target_pos - current_pos) / movement_slowness;
    current_rotation += (target_rotation - current_rotation) / rotation_slowness;
    const bool is_card_visible =
        card->visibility == Card::CardVisibility::front_visible ||
        (card->visibility == Card::CardVisibility::front_visible_to_owner &&
         card->owner_id == game->local_player_id);

    current_size += math::vectors::x_axis *
                    ((is_card_visible ? -texture_width : texture_width) - current_size.x) /
                    flip_slowness;
    current_scale +=
        (((is_being_hovered && !render_darker) ? texture_hovered_scale : texture_scale) -
         current_scale) /
        scale_slowness;

    auto& texture_manager = assets::get_manager<Texture>();
    // Set draw data
    if (current_size.x > 0) {
        auto& back_texture = texture_manager.get_asset(back_texture_handle);
        spr.set_texture(back_texture.handle);
    } else {
        auto& front_texture = texture_manager.get_asset(front_texture_handle);
        spr.set_texture(front_texture.handle);
    }
    spr.set_position(glm::vec2(current_pos.x, current_pos.y));
    spr.set_scale(
        glm::vec2(std::abs(current_size.x) * current_scale, current_size.y * current_scale));
    spr.set_rotation(current_rotation);
    if (render_darker)
        spr.set_color(.7f, .7f, .7f, 1);
    else if (is_being_hovered)
        spr.set_color(1.1f, 1.1f, 1.1f, 1);

    spr.do_draw();
}

math::Rect2D CardSprite::get_rect() {
    math::Vec2D rect_size = current_size * texture_scale;
    return math::Rect2D{current_pos - rect_size.abs() / 2.f, rect_size.abs()};
}

} // namespace renderer
} // namespace munchkin