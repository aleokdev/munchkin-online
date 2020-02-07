#include "systems/game_renderer.hpp"
#include "api/state.hpp"
#include "game.hpp"
#include "game_wrapper.hpp"
#include "render_wrapper.hpp"
#include "renderer/sprite_renderer.hpp"
#include "renderer/util.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <sdl/SDL.h>

namespace munchkin {

namespace systems {

GameRenderer::GameRenderer(RenderWrapper& r) :
    wrapper(&r), camera_buffer(0, 2 * sizeof(float), GL_DYNAMIC_DRAW) {

    auto& texture_manager = assets::get_manager<renderer::Texture>();
    auto& shader_manager = assets::get_manager<renderer::Shader>();

    assets::loaders::LoadParams<renderer::Texture> bg_params;
    bg_params.path = "data/generic/bg.png";
    texture_manager.load_asset("bg", bg_params);
    background = renderer::create_background(texture_manager.get_asset_handle("bg"));

    assets::loaders::LoadParams<renderer::Texture> table_texture_params{"data/generic/table.png"};

    shader_manager.load_asset("sprite_shader",
                              {"data/shaders/sprite.vert", "data/shaders/sprite.frag"});
    sprite_shader = shader_manager.get_asset_handle("sprite_shader");
    table_texture = texture_manager.load_asset("table", table_texture_params);

    // Update camera data
    renderer::UniformBuffer::bind(camera_buffer);
    Game& game = wrapper->wrapper->game;
    camera_buffer.write_data(&game.camera.offset.x, sizeof(float), 0);
    camera_buffer.write_data(&game.camera.offset.y, sizeof(float), sizeof(float));

    update_sprite_vector();
}

GameRenderer::~GameRenderer() { renderer::free_background(background); }

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

    renderer::update_background_scroll(background, delta_time);

    update_camera();

    // Render the background
    renderer::render_background(background);

    auto& texture_manager = assets::get_manager<renderer::Texture>();
    auto& shader_manager = assets::get_manager<renderer::Shader>();

    // Render sprites. Inside block for structure + limiting scope of sprite_renderer
    {
        renderer::SpriteRenderer sprite_renderer;
        // Bind a shader
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
    }
}

void GameRenderer::draw_cards(renderer::SpriteRenderer& spr) {
    for (auto& card : wrapper->wrapper->game.card_sprites) card.draw(spr);
}

} // namespace systems
} // namespace munchkin