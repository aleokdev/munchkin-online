#include "systems/game_renderer.hpp"
#include "api/state.hpp"
#include "game.hpp"
#include "game_wrapper.hpp"
#include "renderer/sprite_renderer.hpp"
#include "renderer/util.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <sdl/SDL.h>

namespace munchkin {

namespace systems {

GameRenderer::GameRenderer(Game& g, GameWrapper& wrapper) :
    game(&g), wrapper(&wrapper), camera_buffer(0, 2 * sizeof(float), GL_DYNAMIC_DRAW),
    title_screen_renderer(wrapper) {

    renderer::RenderTarget::CreateInfo info;
    info.width = game->window_w;
    info.height = game->window_h;

    framebuf = renderer::RenderTarget(info);
    title_screen_renderer.set_render_target(&framebuf);

    auto& texture_manager = assets::get_manager<renderer::Texture>();
    auto& shader_manager = assets::get_manager<renderer::Shader>();

    texture_manager.load_asset("data/cardpacks/default/treasure-back.png",
                               {"data/cardpacks/default/treasure-back.png"});
    texture_manager.load_asset("data/cardpacks/default/dungeon-back.png",
                               {"data/cardpacks/default/dungeon-back.png"});

    background = renderer::create_background(texture_manager.get_asset_handle("bg"));

    assets::loaders::LoadParams<renderer::Texture> table_texture_params{"data/generic/table.png"};

    sprite_shader = shader_manager.get_asset_handle("sprite_shader");
    table_texture = texture_manager.load_asset("table", table_texture_params);

    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    projection = glm::ortho(0.0f, (float)game->window_w, 0.0f, (float)game->window_h);

    // Update camera data
    renderer::UniformBuffer::bind(camera_buffer);
    camera_buffer.write_data(&game->camera.offset.x, sizeof(float), 0);
    camera_buffer.write_data(&game->camera.offset.y, sizeof(float), sizeof(float));

    update_sprite_vector();
}

GameRenderer::~GameRenderer() { renderer::free_background(background); }

void GameRenderer::render_frame() {
    float frame_time = (float)SDL_GetTicks() / 1000.0f;
    delta_time = frame_time - last_frame_time;
    last_frame_time = frame_time;

    update_input();

    // Set viewport to full window
    glViewport(0, 0, framebuf.get_width(), framebuf.get_height());

    renderer::RenderTarget::bind(framebuf);

    framebuf.clear(0, 0, 0, 1, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    TitleScreenRenderer::Status status = TitleScreenRenderer::Status::None;
    switch (state) {
        case State::TitleScreen:
            status = title_screen_renderer.frame(delta_time);
            if (status == TitleScreenRenderer::Status::EnterGamePlaying) {
                state = State::GamePlaying;
                wrapper->do_tick = true;
            }
            if (status == TitleScreenRenderer::Status::QuitApp) {
                wrapper->done = true;
            }
            break;
        case State::GamePlaying: game_playing_frame(); break;
    }

    // Swap current and last mouse
    last_mouse = cur_mouse;
}

void GameRenderer::blit(unsigned int target_framebuf) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target_framebuf);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuf.handle());
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glBlitFramebuffer(0, 0, framebuf.get_width(), framebuf.get_height(), 0, 0, framebuf.get_width(),
                      framebuf.get_height(), GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void GameRenderer::on_resize(size_t w, size_t h) {
    game->window_w = w;
    game->window_h = h;
    framebuf.resize(w, h);
}

void GameRenderer::update_sprite_vector() {
    game->card_sprites.clear();
    for (auto& card : game->get_state().all_cards) {
        game->card_sprites.emplace_back(*game, &card);
    }
}

void GameRenderer::update_input() {
    // update mouse state
    cur_mouse = input::get_current_mouse_state();
}

void GameRenderer::update_camera() {
    // update uniform buffer data
    renderer::UniformBuffer::bind(camera_buffer);
    camera_buffer.write_data(&game->camera.offset.x, sizeof(float), 0);
    camera_buffer.write_data(&game->camera.offset.y, sizeof(float), sizeof(float));
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

        glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(projection));

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
    for (auto& card : game->card_sprites) card.draw(spr);
}

} // namespace systems
} // namespace munchkin