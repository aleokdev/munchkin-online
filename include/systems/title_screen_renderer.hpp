#ifndef MUNCHKIN_TITLE_SCREEN_RENDERER_HPP_
#define MUNCHKIN_TITLE_SCREEN_RENDERER_HPP_

#include "renderer/render_target.hpp"
#include "renderer/background_renderer.hpp"
#include "renderer/assets.hpp"

#include <functional>

namespace munchkin::systems {

class TitleScreenRenderer {
public:
    enum class Status {
        // Regular title screen
        None,
        // Transitioning to GamePlaying state
        TransitionGamePlaying,
        // Signals that the title screen ends, enter the GamePlaying state
        EnterGamePlaying,
        // Signals that the application should quit
        QuitApp
    };

    TitleScreenRenderer();
    void set_render_target(renderer::RenderTarget* tg);

    Status frame(float delta_time);

private:
    using OptionCallbackT = std::function<Status(TitleScreenRenderer&)>;

    renderer::RenderTarget* target;

    renderer::Background background;
    assets::Handle<renderer::Shader> sprite_shader;
    assets::Handle<renderer::Font> font;

    std::vector<std::string> options;
    std::vector<OptionCallbackT> option_callbacks;

    float text_spacing;
    glm::vec2 text_base_position;
    glm::vec2 text_scale;
    static constexpr float base_point_size = 48.0f;
    static constexpr float spacing = 20.0f;

    void render_menu_options();

    float get_menu_option_y_offset(size_t opt_index);
    float calculate_text_width(std::string const& text);

    Status status;

    Status update_status();
};

} // namespace munchkin::renderer

#endif