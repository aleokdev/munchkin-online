#include "renderer/game_renderer.hpp"
#include "renderer/sprite_renderer.hpp"
#include "renderer/util.hpp"
#include "api/game.hpp"

#include <glad/glad.h>
#include <sdl/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace munchkin {

GameRenderer::GameRenderer(Game& g, size_t window_w, size_t window_h) : 
    game(&g), camera_buffer(0, 2 * sizeof(float) + sizeof(glm::mat4), GL_DYNAMIC_DRAW), 
    window_w(window_w), window_h(window_h) {

    renderer::RenderTarget::CreateInfo info;
    info.width = window_w;
    info.height = window_h;

    framebuf = std::move(renderer::RenderTarget(info));
    background = renderer::create_background("data/generic/bg.png");
//    background.scroll_speed = 0.002f;

    sprite_shader = renderer::load_shader("data/shaders/sprite.vert", "data/shaders/sprite.frag");
    table_texture = renderer::load_texture("data/generic/table.png");

    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    projection = glm::ortho(0.0f, (float)window_w, 0.0f, (float)window_h);
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

    renderer::SpriteRenderer sprite_renderer;
    // Bind a shader
    glUseProgram(sprite_shader);

    glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(projection));

    // Set draw data    
    sprite_renderer.set_camera_drag(true);
    sprite_renderer.set_texture(table_texture);
    sprite_renderer.set_position(glm::vec2(400, 200));
    sprite_renderer.set_scale(glm::vec2(300, 300));

    // Execute drawcall
    sprite_renderer.do_draw();

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

void GameRenderer::update_input() {
    // update mouse state
    cur_mouse.pressed_button = SDL_GetMouseState(&cur_mouse.x, &cur_mouse.y);
}

void GameRenderer::update_camera() {
    constexpr float pan_speed = 0.1f;

    // only enable panning if left mouse button is clicked
    if (cur_mouse.pressed_button & SDL_BUTTON_LEFT) {
        int xoffset = cur_mouse.x - last_mouse.x;
        int yoffset = cur_mouse.y - last_mouse.y;

        camera.xoffset -= xoffset * pan_speed * delta_time;
        camera.yoffset += yoffset * pan_speed * delta_time;

        // update uniform buffer data
        renderer::UniformBuffer::bind(camera_buffer);
        camera_buffer.write_data(&camera.xoffset, sizeof(float), 0);
        camera_buffer.write_data(&camera.yoffset, sizeof(float), sizeof(float));
    }
}

}