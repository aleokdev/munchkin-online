#ifndef MUNCHKIN_GAME_RENDERER_HPP__
#define MUNCHKIN_GAME_RENDERER_HPP__

#include "assets/assets.hpp"
#include "input/input.hpp"
#include "renderer/background_renderer.hpp"
#include "renderer/render_target.hpp"
#include "renderer/sprite_renderer.hpp"
#include "renderer/uniform_buffer.hpp"
#include "sound/sound_assets.hpp"
#include "util/pos_vec.hpp"
#include "sol/sol.hpp"
#include <unordered_set>
#include <stack>

#include <api/card.hpp>
#include <glm/mat4x4.hpp>

namespace munchkin {

class Game;
class RenderWrapper;
struct FlowEvent;

namespace renderer {
class CardSprite;
}

namespace systems {

class GameRenderer {
public:
    GameRenderer(RenderWrapper&);

    [[call_after_load(renderer)]]
    void load_content();

    void render();

    void update_sprite_vector();

    // Yield callbacks

    // Syntax: `coroutine.yield("choose_card", filter)`
    // Where `filter` is a function that returns true/false for a given card.
    // Must return true if the card should be choosable, false otherwise.
    void on_choose_card_yield(sol::coroutine const&, sol::protected_function_result const& yield_return_result);

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
    assets::Handle<renderer::Shader> solid_shader;
    assets::Handle<renderer::Shader> text_shader;

    assets::Handle<renderer::Font> infobar_title_font;
    assets::Handle<renderer::Font> infobar_normal_font;

    assets::Handle<renderer::Texture> table_texture;
    assets::Handle<sound::Music> game_music;

    // Info box
    std::string info_box_title;
    std::string info_box_description;
    math::Vec2D info_box_position;
    static constexpr float info_box_move_slowness = 16;
    renderer::CardSprite* last_frame_hovered_sprite = nullptr;

    // Functions
    void update_camera();
    void update_input();
    void game_playing_frame();
    void draw_cards(renderer::SpriteRenderer&);

    // choose_card yield callback
    struct ChooseCardPopup {
        sol::coroutine caller;
        std::unordered_set<CardPtr> choosable_cards;
    };

    std::stack<ChooseCardPopup> choose_card_popups;
};

} // namespace systems
} // namespace munchkin

#endif