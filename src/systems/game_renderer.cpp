#include "systems/game_renderer.hpp"
#include "renderer/sprite_renderer.hpp"
#include "renderer/util.hpp"
#include "game.hpp"
#include "api/state.hpp"

#include <glad/glad.h>
#include <sdl/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace munchkin {

namespace systems {

GameRenderer::GameRenderer(Game& g) :
    game(&g), camera_buffer(0, 2 * sizeof(float), GL_DYNAMIC_DRAW) {

    renderer::RenderTarget::CreateInfo info;
    info.width = game->window_w;
    info.height = game->window_h;

    framebuf = renderer::RenderTarget(info);
    background = renderer::create_background("data/generic/bg.png");
    //    background.scroll_speed = 0.002f;

    sprite_shader = renderer::load_shader("data/shaders/sprite.vert", "data/shaders/sprite.frag");
    table_texture = renderer::load_texture("data/generic/table.png");

    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    projection = glm::ortho(0.0f, (float)game->window_w, 0.0f, (float)game->window_h);

    // Update camera data
    renderer::UniformBuffer::bind(camera_buffer);
    camera_buffer.write_data(&game->camera.offset.x, sizeof(float), 0);
    camera_buffer.write_data(&game->camera.offset.y, sizeof(float), sizeof(float));

    // TODO FIXME: This doesn't add cards that are added AFTER the gamerenderer has been created!
    for (auto& card : game->get_state().all_cards) {
        game->card_sprites.emplace_back(&card);
    }
}

GameRenderer::~GameRenderer() {
    renderer::free_background(background);
    glDeleteProgram(sprite_shader);
}

void GameRenderer::render_frame() {
    float frame_time = (float)SDL_GetTicks() / 1000.0f;
    delta_time = frame_time - last_frame_time;
    last_frame_time = frame_time;

    // Update stuff
    update_input();

    renderer::update_background_scroll(background, delta_time);

    update_camera();

    // Set viewport to full window
    glViewport(0, 0, framebuf.get_width(), framebuf.get_height());

    renderer::RenderTarget::bind(framebuf);

    framebuf.clear(0, 0, 0, 1, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render the background
    renderer::render_background(background);

    // Render sprites. Inside block for structure + limiting scope of sprite_renderer
    {
        renderer::SpriteRenderer sprite_renderer;
        // Bind a shader
        glUseProgram(sprite_shader);

        glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(projection));

        // Set draw data    
        sprite_renderer.set_camera_drag(true);
        sprite_renderer.set_texture(table_texture);
        constexpr float table_size = 1400;
        // Calculate position for lower left corner for the table to be centered
        sprite_renderer.set_position(glm::vec2(0, 0));
        sprite_renderer.set_scale(glm::vec2(table_size, table_size));
        // Execute drawcall
        sprite_renderer.do_draw();

        draw_cards(sprite_renderer);

    }

    // Swap current and last mouse
    last_mouse = cur_mouse;
}

void GameRenderer::blit(unsigned int target_framebuf) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target_framebuf);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuf.handle());
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glBlitFramebuffer(0, 0, framebuf.get_width(), framebuf.get_height(),
        0, 0, framebuf.get_width(), framebuf.get_height(),
        GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void GameRenderer::on_resize(size_t w, size_t h) {
    game->window_w = w;
    game->window_h = h;

    framebuf.resize(w, h);
}

void GameRenderer::update_input() {
    // update mouse state
    cur_mouse = input::get_current_mouse_state();
}

void GameRenderer::update_camera() {
    constexpr float pan_speed = 0.1f;

    // only enable panning if left mouse button is clicked
    if (cur_mouse.button_flagmap & SDL_BUTTON(SDL_BUTTON_LEFT)
        & last_mouse.button_flagmap & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        int xoffset = cur_mouse.x - last_mouse.x;
        int yoffset = cur_mouse.y - last_mouse.y;

        game->camera.offset.x -= xoffset * pan_speed * delta_time;
        game->camera.offset.y += yoffset * pan_speed * delta_time;

        // update uniform buffer data
        renderer::UniformBuffer::bind(camera_buffer);
        camera_buffer.write_data(&game->camera.offset.x, sizeof(float), 0);
        camera_buffer.write_data(&game->camera.offset.y, sizeof(float), sizeof(float));
    }
}

void GameRenderer::draw_cards(renderer::SpriteRenderer& spr) {
    for (auto& card : game->card_sprites)
        card.draw(spr);
}

}
}