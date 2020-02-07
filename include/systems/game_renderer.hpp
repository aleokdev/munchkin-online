#ifndef MUNCHKIN_GAME_RENDERER_HPP__
#define MUNCHKIN_GAME_RENDERER_HPP__

#include "input/input.hpp"
#include "renderer/assets.hpp"
#include "renderer/background_renderer.hpp"
#include "renderer/render_target.hpp"
#include "renderer/sprite_renderer.hpp"
#include "renderer/uniform_buffer.hpp"

#include <glm/mat4x4.hpp>

namespace munchkin {

class Game;
class RenderWrapper;

namespace systems {

class GameRenderer {
public:
    GameRenderer(RenderWrapper&);

    void render();

    void update_sprite_vector();

private:
    RenderWrapper* wrapper;

    input::MouseState last_mouse;
    input::MouseState cur_mouse;

    // Timing
    float delta_time;
    float last_frame_time = 0;

    // Render data
    renderer::UniformBuffer camera_buffer;

    // Assets
    renderer::Background background;
    assets::Handle<renderer::Shader> sprite_shader;
    assets::Handle<renderer::Texture> table_texture;

    // Functions
    void update_camera();
    void update_input();
    void game_playing_frame();
    void draw_cards(renderer::SpriteRenderer&);
};

} // namespace systems
} // namespace munchkin

#endif