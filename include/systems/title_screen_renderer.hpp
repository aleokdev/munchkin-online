#ifndef MUNCHKIN_TITLE_SCREEN_RENDERER_HPP_
#define MUNCHKIN_TITLE_SCREEN_RENDERER_HPP_

#include "assets/assets.hpp"
#include "renderer/background_renderer.hpp"
#include "renderer/render_target.hpp"
#include "sound/sound_assets.hpp"

#include <filesystem>
#include <functional>
#include <optional>

namespace fs = std::filesystem;

namespace munchkin {
class RenderWrapper;

namespace systems {

class [[system_codegen::renderer_instance("title_screen_renderer")]] TitleScreenRenderer {
public:
    enum class Status {
        // Regular title screen
        None,
        // Transitioning to GamePlaying state [Not actually used]
        TransitionGamePlaying,
        // Signals that the title screen ends, enter the GamePlaying state
        EnterGamePlaying,
        // Signals that we are displaying the credits screen
        Credits,
        // Signals that we are displaying the initial loading screen
        LoadingAssets,
        // Signals that we're transitioning from LoadingAssets to None
        ExitLoadingAssets,
        // Signals that the application should quit
        QuitApp
    };

    TitleScreenRenderer(::munchkin::RenderWrapper& wrapper);

    void load_content();

    void set_render_target(renderer::RenderTarget* tg);

    void show_loading_screen();
    std::function<void()> on_load_assets_finish = nullptr;

    Status frame(float delta_time);

private:
    using OptionCallbackT = std::function<Status()>;

    ::munchkin::RenderWrapper* wrapper;
    renderer::RenderTarget* target;

    // Assets
    renderer::Background static_bg;
    renderer::Background dynamic_bg;
    assets::Handle<renderer::Texture> logo_texture;

    assets::Handle<renderer::Shader> sprite_shader;
    assets::Handle<renderer::Shader> solid_shader;
    assets::Handle<renderer::Font> font;

    assets::Handle<sound::SoundEffect> hover_sfx;
    assets::Handle<sound::SoundEffect> click_sfx;

    // Menu option parameters & struct
    struct MenuOption {
        std::string name;
        OptionCallbackT callback;
        glm::vec4 color;
        // When selected, a menu option is assigned a scale offset so it appears selected
        float scale = 1.0f;
        bool selected = false;
        float text_width;

        MenuOption(std::string n, OptionCallbackT c, glm::vec3 col, float _text_width) :
            name(n), callback(c), text_width(_text_width) {}
    };
    static constexpr float selected_option_scale = 1.2f;
    static constexpr float offset_animate_slowness = 8.0f;
    std::vector<MenuOption> options;
    static constexpr glm::vec4 default_option_color = glm::vec4(0.53, 0.53, 0.53, 1);
    void add_main_menu_options();

    // Transitions
    static constexpr float exit_loading_screen_transition_time = 2.5f;
    // The time it takes for the logo to move from the center to the top after changing from the
    // LoadingAssets status.
    static constexpr float logo_translation_time = 1.5f;
    static constexpr float loading_bar_fade_time = .5f;
    // The time it takes for the menu options to start appearing.
    static constexpr float options_appear_time = 1.6f;

    // Text parameters
    float text_spacing;
    glm::vec2 text_base_position;
    glm::vec2 text_scale;
    static constexpr float base_point_size = 48.0f;
    static constexpr float spacing = 20.0f;

    // Render functions
    void render_background(float delta_time);
    void render_logo(float delta_time);
    void render_options(float delta_time);
    void render_loading_bar(float delta_time);

    float get_menu_option_y_offset(size_t opt_index);

    Status status = Status::None;
    float time_since_last_status_change = 0;

    Status update_status(float delta_time);

    // Game settings
    struct GameSettings {
        size_t total_players = 3;

        std::string local_player_name;

        std::optional<fs::path> ai_path;
        std::optional<fs::path> gamerules_path;
        std::vector<fs::path> cardpack_paths;
    };
    GameSettings game_settings;
    bool game_settings_opened = false;

    // Loading screen
    class LoadingScreenData {
    public:
        LoadingScreenData(assets::AssetReferenceContainer const& asset_container) :
            asset_container(asset_container) {}

        /// Returns the next asset to load, or nullptr if loaded everything.
        assets::AssetRef* next_asset_to_load() noexcept {
            if (current_asset_loading_index == asset_container.size() - 1)
                return nullptr;

            return &asset_container[current_asset_loading_index++];
        }

        /// Returns a float between 0 and 1 detailing the loading progress (0 is not loaded
        /// anything, 1 is loaded everything)
        float load_progress() const noexcept {
            return (float)current_asset_loading_index / (float)asset_container.size();
        }

    private:
        assets::AssetReferenceContainer asset_container;
        std::size_t current_asset_loading_index = 0;
    };
    std::optional<LoadingScreenData> loading_screen_data;
    constexpr static float loading_bar_width = 400;
    constexpr static float loading_bar_height = 20;
    constexpr static float loading_bar_border_size = 2;

    // Button callbacks
    TitleScreenRenderer::Status quit();
    TitleScreenRenderer::Status local_game();
    TitleScreenRenderer::Status credits();
    TitleScreenRenderer::Status exit_credits();
};

} // namespace systems
} // namespace munchkin

#endif