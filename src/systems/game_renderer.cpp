#include "systems/game_renderer.hpp"
#include "api/state.hpp"
#include "game.hpp"
#include "game_wrapper.hpp"
#include "render_wrapper.hpp"
#include "renderer/sprite_renderer.hpp"
#include "renderer/util.hpp"
#include "sound/sound_player.hpp"

#include <audeo/audeo.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#ifdef _WIN32
#    include <sdl/SDL.h>
#else
#    include <SDL2/SDL.h>
#endif

namespace munchkin {

namespace systems {

GameRenderer::GameRenderer(RenderWrapper& r) :
    wrapper(&r), camera_buffer(0, 2 * sizeof(float), GL_DYNAMIC_DRAW),
    background(assets::get_manager<renderer::Texture>().load_asset(
        "bg", renderer::Background::default_load_params)) {

    auto& texture_manager = assets::get_manager<renderer::Texture>();
    auto& shader_manager = assets::get_manager<renderer::Shader>();
    auto& font_manager = assets::get_manager<renderer::Font>();
    auto& music_manager = assets::get_manager<assets::Music>();

    assets::loaders::LoadParams<renderer::Texture> table_texture_params{"data/generic/table.png"};

    sprite_shader = shader_manager.load_asset(
        "sprite_shader", {"data/shaders/sprite.vert", "data/shaders/sprite.frag"});
    solid_shader = shader_manager.load_asset(
        "solid_shader", {"data/shaders/solid.vert", "data/shaders/solid.frag"});

    infobar_title_font =
        font_manager.load_asset("main_font", {"data/generic/quasimodo_regular.ttf"});
    infobar_normal_font =
        font_manager.load_asset("medium_sec_font", {"data/generic/Raleway-Medium.ttf"});
    table_texture = texture_manager.load_asset("table", table_texture_params);
    game_music = music_manager.load_asset("game_song", {"data/generic/game_song.mp3"});

    // Update camera data
    renderer::UniformBuffer::bind(camera_buffer);
    Game& game = wrapper->wrapper->game;
    camera_buffer.write_data(&game.camera.offset.x, sizeof(float), 0);
    camera_buffer.write_data(&game.camera.offset.y, sizeof(float), sizeof(float));

    // Update infobox position
    info_box_position = math::Vec2D{(float)wrapper->wrapper->game.window_w,
                                    (float)wrapper->wrapper->game.window_h / 2.f};

    update_sprite_vector();
}

void GameRenderer::render() {
    float frame_time = (float)SDL_GetTicks() / 1000.0f;
    delta_time = frame_time - last_frame_time;
    last_frame_time = frame_time;

    update_input();
    game_playing_frame();

    // Swap current and last mouse
    last_mouse = cur_mouse;
}

void GameRenderer::update_sprite_vector() {
    Game& game = wrapper->wrapper->game;
    game.card_sprites.clear();
    for (auto& card : game.get_state().all_cards) { game.card_sprites.emplace_back(game, &card); }
}

void GameRenderer::update_input() {
    // update mouse state
    cur_mouse = input::get_current_mouse_state();
}

void GameRenderer::update_camera() {
    // update uniform buffer data
    renderer::UniformBuffer::bind(camera_buffer);
    Game& game = wrapper->wrapper->game;
    camera_buffer.write_data(&game.camera.offset.x, sizeof(float), 0);
    camera_buffer.write_data(&game.camera.offset.y, sizeof(float), sizeof(float));
}

void GameRenderer::game_playing_frame() {
    static bool first_time = true;
    update_camera();

    // Render the background
    background.render();

    auto& music_manager = assets::get_manager<assets::Music>();
    if(first_time && !audeo::is_playing_music())
    {
        sound::play_music(game_music);
        first_time = false;
    }

    auto& texture_manager = assets::get_manager<renderer::Texture>();
    auto& shader_manager = assets::get_manager<renderer::Shader>();

    // Render sprites. Inside block for structure + limiting scope of sprite_renderer
    {
        renderer::SpriteRenderer sprite_renderer;
        // Bind sprite shader
        auto& shader = shader_manager.get_asset(sprite_shader);
        glUseProgram(shader.handle);

        glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(wrapper->projection));

        // Set draw data
        sprite_renderer.set_camera_drag(true);
        auto& tex = texture_manager.get_asset(table_texture);
        sprite_renderer.set_texture(tex.handle);
        constexpr float table_size = renderer::table_radius * 2.f;
        // Calculate position for lower left corner for the table to be centered
        sprite_renderer.set_position(glm::vec2(0, 0));
        sprite_renderer.set_scale(glm::vec2(table_size, table_size));
        // Execute drawcall
        sprite_renderer.do_draw();

        draw_cards(sprite_renderer);

        // Draw current hovered sprite info
        const auto hovered_sprite = wrapper->wrapper->game.current_hovered_sprite;
        if (hovered_sprite &&
            (hovered_sprite->get_card_ptr()->visibility == Card::CardVisibility::front_visible ||
             (hovered_sprite->get_card_ptr()->is_being_owned_by_player() &&
              hovered_sprite->get_card_ptr()->owner_id ==
                  wrapper->wrapper->game.local_player_id))) {
            info_box_title = hovered_sprite->get_card_ptr()->get_def().name;
            info_box_description = hovered_sprite->get_card_ptr()->get_def().description;

            last_frame_hovered_sprite = hovered_sprite;

        } else
            last_frame_hovered_sprite = nullptr;

        // Bind solid shader
        auto& s_shader = shader_manager.get_asset(solid_shader);
        glUseProgram(s_shader.handle);
        glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(wrapper->projection));

        const auto window_w = wrapper->wrapper->game.window_w;
        const auto window_h = wrapper->wrapper->game.window_h;
        info_box_position += math::Vec2D{
            ((last_frame_hovered_sprite ? window_w : window_w * 2.f) - info_box_position.x) /
                info_box_move_slowness,
            0};
        sprite_renderer.set_camera_drag(false);
        sprite_renderer.set_position(info_box_position);
        sprite_renderer.set_scale(glm::vec2(window_w - 400, window_h - 200));
        sprite_renderer.set_color(0.1f, 0.1f, 0.1f, 0.7f);

        // Execute drawcall
        sprite_renderer.do_draw();

        renderer::FontRenderer font_renderer;
        font_renderer.set_position(math::Vec2D{info_box_position.x/(float)window_w-.33f, .16f});
        font_renderer.set_size(glm::vec2(0.6f, 0.6f));
        font_renderer.set_window_size(window_w, window_h);
        font_renderer.render_text(infobar_title_font, info_box_title);

        font_renderer.set_position(
            math::Vec2D{info_box_position.x/(float)window_w-.33f, .21f});
        font_renderer.set_size(glm::vec2(0.3f, 0.3f));
        font_renderer.set_window_size(window_w, window_h);
        font_renderer.render_text(infobar_normal_font, info_box_description);
    }
}

void GameRenderer::draw_cards(renderer::SpriteRenderer& spr) {
    for (auto& card : wrapper->wrapper->game.card_sprites) card.draw(spr);
}

} // namespace systems
} // namespace munchkin