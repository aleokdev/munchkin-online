#include "renderer/card_sprite.hpp"
#include "api/state.hpp"
#include "game.hpp"
#include "game_wrapper.hpp"
#include "render_wrapper.hpp"
#include "renderer/font_renderer.hpp"
#include "renderer/sprite_renderer.hpp"
#include "renderer/util.hpp"
#include "sound/sound_assets.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#ifdef _WIN32
#    include <sdl/SDL.h>
#else
#    include <SDL2/SDL.h>
#endif

using namespace munchkin::renderer::internal::card_sprite;

namespace fs = std::filesystem;

namespace munchkin {
namespace renderer {

CardSprite::CardSprite(RenderWrapper& g, CardPtr _card) : wrapper(&g), card(_card) {
    auto& texture_manager = assets::get_manager<Texture>();
    back_texture_handle = texture_manager.load_asset(card->get_def().back_texture_path,
                                                     {card->get_def().back_texture_path});
    front_texture_handle = texture_manager.load_asset(card->get_def().front_texture_path,
                                                      {card->get_def().front_texture_path});
    if (!initialized_assets) {
        auto& sfx_manager = assets::get_manager<sound::SoundEffect>();
        move_sfx = sfx_manager.load_asset("card_deal", {"data/generic/card_deal.ogg"});
        flip_sfx = sfx_manager.load_asset("card_flip", {"data/generic/card_flip.ogg"});
        auto& font_manager = assets::get_manager<renderer::Font>();
        monster_power_font =
            font_manager.load_asset("main_font", {"data/generic/quasimodo_regular.ttf"});
    }
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
                ((float)card_owner.get_id()) / ((float)card.state->players.size()) * 2.f * M_PI -
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
                ((float)card_owner.get_id()) / ((float)card.state->players.size()) * 2.f * M_PI -
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
                const int battle_cards =
                    wrapper->wrapper->game.state.current_battle->played_cards.size();
                int card_index = 0;
                for (auto& [k, v] : wrapper->wrapper->game.state.current_battle->played_cards) {
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
    const auto& game = wrapper->wrapper->game;
    if (last_card_location != card->get_location() ||
        card.state->players[card->owner_id].hand.size() != last_cards_in_owner) {
        calculate_target_from_location();
        if (!first_sfx_play && last_card_location != card->get_location()) {
            auto& sfx_manager = assets::get_manager<sound::SoundEffect>();
            audeo::Sound s = audeo::play_sound(sfx_manager.get_asset(CardSprite::move_sfx).source);
            audeo::set_volume(s, .5f);
        }
        last_card_location = card->get_location();
        last_cards_in_owner = card.state->players[card->owner_id].hand.size();
    }

    bool render_darker = false;
    if (card->is_being_owned_by_player() &&
        card.state->get_game_stage() != card.state->get_last_game_stage()) {
        render_darker =
            card->owner_id == game.local_player_id &&
            std::find(card->get_def().play_stages.begin(), card->get_def().play_stages.end(),
                      card.state->get_game_stage()) == card->get_def().play_stages.end();
    }

    // Only render highlighted if hovered and being owned by local player or not being owned by
    // anyone
    bool render_highlighted =
        is_being_hovered &&
        ((card->is_being_owned_by_player() && card->owner_id == game.local_player_id) ||
         !card->is_being_owned_by_player());

    current_pos += (target_pos - current_pos) / movement_slowness;
    current_rotation += (target_rotation - current_rotation) / rotation_slowness;
    const bool is_card_visible =
        card->visibility == Card::CardVisibility::front_visible ||
        (card->visibility == Card::CardVisibility::front_visible_to_owner &&
         card->owner_id == game.local_player_id);

    if (!first_sfx_play && (is_card_visible ^ last_visible)) {
        auto& sfx_manager = assets::get_manager<sound::SoundEffect>();
        audeo::Sound s = audeo::play_sound(sfx_manager.get_asset(CardSprite::flip_sfx).source);
        audeo::set_volume(s, .5f);
    }
    last_visible = is_card_visible;

    current_size += math::vectors::x_axis *
                    ((is_card_visible ? -texture_width : texture_width) - current_size.x) /
                    flip_slowness;
    current_scale +=
        (((render_highlighted && !render_darker) ? texture_hovered_scale : texture_scale) -
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
    else if (render_highlighted)
        spr.set_color(1.1f, 1.1f, 1.1f, 1);

    spr.do_draw();

    if (last_card_location == Card::CardLocation::table_center && card->get_def().is_monster &&
        game.state.current_battle && game.state.current_battle->is_card_played(card)) {
        // draw cardpower below
        std::string power_str = std::to_string(game.state.current_battle->get_card_power(card));

        renderer::FontRenderer fnt;
        fnt.set_window_size(game.window_w, game.window_h);
        fnt.set_size(glm::vec2{1, 1});
        fnt.set_color({1, 1, 1});
        const auto& font(assets::get_manager<renderer::Font>().get_asset(monster_power_font));
        const math::Vec2D pixel_pos(
            (get_rect().pos * math::Vec2D{1, -1} + get_rect().size * math::Vec2D{.5f, 0}) -
            game.camera.offset * math::Vec2D{(float)game.window_w, -(float)game.window_h} / 2.f +
            math::Vec2D{0, (float)game.window_h});
        const float textWidth = font.calculate_width(power_str);
        const math::Vec2D final_pos((pixel_pos - math::Vec2D{textWidth / 2.f, 0}) /
                                    math::Vec2D{(float)game.window_w, (float)game.window_h});

        fnt.set_position(final_pos);
        fnt.render_text(monster_power_font, power_str);

        // Set up everything again for rendering sprites
        auto& sm = assets::get_manager<renderer::Shader>();
        glUseProgram(sm.get_asset(sm.get_asset_handle("sprite_shader")).handle);
        glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(wrapper->projection));
    }

    first_sfx_play = false;
}

math::Rect2D CardSprite::get_rect() {
    math::Vec2D rect_size = current_size * texture_scale;
    return math::Rect2D{current_pos - rect_size.abs() / 2.f, rect_size.abs()};
}

} // namespace renderer
} // namespace munchkin