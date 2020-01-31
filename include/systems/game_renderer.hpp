#ifndef MUNCHKIN_GAME_RENDERER_HPP__
#define MUNCHKIN_GAME_RENDERER_HPP__

#include "input/input.hpp"
#include "renderer/background_renderer.hpp"
#include "renderer/render_target.hpp"
#include "renderer/sprite_renderer.hpp"
#include "renderer/uniform_buffer.hpp"
#include "renderer/assets.hpp"

#include <glm/mat4x4.hpp>

namespace munchkin {

class Game;

namespace systems {

class GameRenderer {
public:
    GameRenderer(Game&);
    ~GameRenderer();

    void render_frame();
    void blit(unsigned int target_framebuf);

    void on_resize(size_t w, size_t h);

    void update_sprite_vector();

private:
    glm::mat4 projection;

    // Game-related data
    Game* game;

    input::MouseState last_mouse;
    input::MouseState cur_mouse;

    // Timing
    float delta_time;
    float last_frame_time = 0;

    // Render data
    renderer::RenderTarget framebuf;
    renderer::UniformBuffer camera_buffer;

    // Assets
    renderer::Background background;
    assets::Handle<renderer::Shader> sprite_shader;
    assets::Handle<renderer::Texture> table_texture;

    // Functions
    void update_camera();
    void update_input();
    void draw_cards(renderer::SpriteRenderer&);
};

} // namespace systems
} // namespace munchkin

#endif