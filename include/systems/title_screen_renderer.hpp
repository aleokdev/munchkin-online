#ifndef MUNCHKIN_TITLE_SCREEN_RENDERER_HPP_
#define MUNCHKIN_TITLE_SCREEN_RENDERER_HPP_

#include "renderer/assets.hpp"
#include "renderer/background_renderer.hpp"
#include "renderer/render_target.hpp"

#include <filesystem>
#include <functional>
#include <optional>

namespace fs = std::filesystem;

namespace munchkin {
class GameWrapper;

namespace systems {

class TitleScreenRenderer {
public:
    enum class Status {
        // Regular title screen
        None,
        // Transitioning to GamePlaying state
        TransitionGamePlaying,
        // Signals that the title screen ends, enter the GamePlaying state
        EnterGamePlaying,
        // Signals that we are displaying the credits screen
        Credits,
        // Signals that the application should quit
        QuitApp
    };

    TitleScreenRenderer(::munchkin::GameWrapper& wrapper);
    void set_render_target(renderer::RenderTarget* tg);

    Status frame(float delta_time);

private:
    using OptionCallbackT = std::function<Status(TitleScreenRenderer&)>;

    renderer::RenderTarget* target;

    renderer::Background background;
    assets::Handle<renderer::Shader> sprite_shader;
    assets::Handle<renderer::Font> font;
    ::munchkin::GameWrapper* wrapper;
    struct MenuOption {
        std::string name;
        OptionCallbackT callback;
        glm::vec3 color;
        // When selected, a menu option is assigned an offset so it appears selected
        float offset = 0.0f;
    };

public:
    std::vector<MenuOption> options;

    static constexpr glm::vec3 default_option_color = glm::vec3(0.53, 0.53, 0.53);

    bool game_settings_opened = false;

private:
    static constexpr float selected_option_offset = 20.0f;
    static constexpr float offset_animate_slowness = 8.0f;

    float text_spacing;
    glm::vec2 text_base_position;
    glm::vec2 text_scale;
    static constexpr float base_point_size = 48.0f;
    static constexpr float spacing = 20.0f;

    void render_menu_options();
    void render_credits();

    float get_menu_option_y_offset(size_t opt_index);
    float calculate_text_width(std::string const& text);

    Status status = Status::None;

    Status update_status(float delta_time);

    struct GameSettings {
        size_t total_players = 3;
        size_t total_ai_players = 2;

        std::string local_player_name;

        std::optional<fs::path> ai_path;
        std::optional<fs::path> gamerules_path;
        std::vector<fs::path> cardpack_paths;
    };

    GameSettings game_settings;
};

} // namespace systems
} // namespace munchkin

#endif