#include "systems/title_screen_renderer.hpp"

#include "assets/assets.hpp"
#include "input/input.hpp"
#include "renderer/font_renderer.hpp"
#include "util/util.hpp"

#include "sound/sound_player.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include <algorithm>

#include "game_wrapper.hpp"
#include "render_wrapper.hpp"
#include "sound/sound_assets.hpp"

/// Smoothly interpolates between 0 and 1 given a linear t between 0 and 1 inclusive.
static float sigmoid(float t) { return 1.f / (1.f + std::pow(M_E, -(t * 12.f - 6.f))); }

namespace munchkin {
namespace systems {

// TODO: Separate loading screen from title screen
TitleScreenRenderer::TitleScreenRenderer(::munchkin::RenderWrapper& _wrapper) :
    wrapper(&_wrapper), static_bg(assets::AssetManager::load_asset<renderer::Texture>("vignette")),
    dynamic_bg(assets::AssetManager::load_asset<renderer::Texture>("noise"), true) {

    // Load assets

    sprite_shader = assets::AssetManager::load_asset<renderer::Shader>("sprite_shader");
    solid_shader = assets::AssetManager::load_asset<renderer::Shader>("solid_shader");
    logo_texture = assets::AssetManager::load_asset<renderer::Texture>("logo");

    hover_sfx = assets::AssetManager::load_asset<sound::SoundEffect>("ui_hover_sfx");
    click_sfx = assets::AssetManager::load_asset<sound::SoundEffect>("ui_click_sfx");

    font = assets::AssetManager::load_asset<renderer::Font>("title_font");

    text_scale = glm::vec2(1, 1);
    text_base_position = glm::vec2(0.5f, 0.4f);

    add_main_menu_options();
}

void TitleScreenRenderer::load_content() {}

void TitleScreenRenderer::set_render_target(renderer::RenderTarget* tg) {
    target = tg;
    text_spacing = (text_scale.y * base_point_size + spacing) / target->get_height();
}

TitleScreenRenderer::Status TitleScreenRenderer::frame(float delta_time) {
    time_since_last_status_change += delta_time;

    render_background(delta_time);
    render_logo(delta_time);
    render_loading_bar(delta_time);

    if (status == Status::LoadingAssets) {
        auto* next_asset_to_load = loading_screen_data.value().next_asset_to_load();

        if (next_asset_to_load) {
            next_asset_to_load->load();
        } else {
            // Done loading all assets!
            add_main_menu_options();
            on_load_assets_finish();
            loading_screen_data.reset();
            status = Status::ExitLoadingAssets;
            time_since_last_status_change = 0;
        }
    }

    // Start the title music if we're not loading assets
    if (status == Status::None && !sound::get_current_music_handle_being_played()) {
        sound::play_music(assets::AssetManager::load_asset<sound::Music>("title_song"));
        sound::set_music_volume(0.4f);
    }

    if (status == Status::ExitLoadingAssets &&
        time_since_last_status_change > exit_loading_screen_transition_time) {
        // Transition to main menu
        status = Status::None;
        time_since_last_status_change = 0;
    }

    render_options(delta_time);

    if (game_settings_opened) // OpenPopup needs to be used before the popup window and there can't
                              // be a new frame in between both
        ImGui::OpenPopup("Setup Game...");

    // TODO: Clean this up.
    if (ImGui::BeginPopupModal("Setup Game...", &game_settings_opened,
                               ImGuiWindowFlags_AlwaysAutoResize)) {
        { // Local player name
            static char player_name_buf[32] = "Player";

            ImGui::InputText("Player name", player_name_buf, 32);

            game_settings.local_player_name = player_name_buf;
        }

        { // Total players slider
            int total_players = game_settings.total_players;
            ImGui::InputInt("Total players", &total_players);
            game_settings.total_players = std::max(0, total_players);
        }

        { // Gamerules combo box
            static bool update_available_vector = true;
            static std::vector<fs::path> gamerules_available;

            if (update_available_vector) {
                gamerules_available.clear();
                for (const auto& entry : fs::directory_iterator("data/gamerules/"))
                    gamerules_available.emplace_back(entry);

                if (gamerules_available.size() == 0)
                    game_settings.gamerules_path.reset();
                else
                    game_settings.gamerules_path = gamerules_available[0];

                update_available_vector = false;
            }

            std::string preview_str =
                game_settings.gamerules_path
                    ? game_settings.gamerules_path->filename().generic_string()
                    : "Select one...";
            if (ImGui::BeginCombo("Gamerules", preview_str.c_str())) {
                for (const auto& entry : gamerules_available) {
                    std::string entry_name = entry.filename().generic_string();
                    if (ImGui::Selectable(entry_name.c_str(),
                                          entry == game_settings.gamerules_path))
                        game_settings.gamerules_path = entry;
                }
                ImGui::EndCombo();
            }
        }

        { // AI combo box
            static bool update_available_vector = true;
            static std::vector<fs::path> ais_available;

            if (update_available_vector) {
                ais_available.clear();
                for (const auto& entry : fs::directory_iterator("data/ai/"))
                    ais_available.emplace_back(entry);

                if (ais_available.size() == 0)
                    game_settings.ai_path.reset();
                else
                    game_settings.ai_path = ais_available[0];

                update_available_vector = false;
            }

            std::string preview_str = game_settings.ai_path
                                          ? game_settings.ai_path->filename().generic_string()
                                          : "Select one...";
            if (ImGui::BeginCombo("AI Scripting", preview_str.c_str())) {
                for (const auto& entry : ais_available) {
                    std::string entry_name = entry.filename().generic_string();
                    if (ImGui::Selectable(entry_name.c_str(), entry == game_settings.ai_path))
                        game_settings.ai_path = entry;
                }
                ImGui::EndCombo();
            }
        }

        { // Cardpacks combo box
            static bool update_available_vector = true;
            static std::vector<fs::path> cardpacks_available;

            if (update_available_vector) {
                cardpacks_available.clear();
                for (const auto& entry : fs::directory_iterator("data/cardpacks/"))
                    cardpacks_available.emplace_back(entry);

                for (auto& cardpack : cardpacks_available)
                    game_settings.cardpack_paths.emplace_back(cardpack);

                update_available_vector = false;
            }

            ImGui::TextUnformatted("Cardpacks");
            if (ImGui::BeginChildFrame(ImGui::GetID("_Cardpacks"), {0, 200})) {
                for (const auto& entry : cardpacks_available) {
                    std::string entry_name = entry.filename().generic_string();
                    bool enabled = std::find(game_settings.cardpack_paths.begin(),
                                             game_settings.cardpack_paths.end(),
                                             entry) != game_settings.cardpack_paths.end();
                    if (ImGui::Selectable(entry_name.c_str(), enabled)) {
                        if (enabled) {
                            game_settings.cardpack_paths.erase(
                                std::remove(game_settings.cardpack_paths.begin(),
                                            game_settings.cardpack_paths.end(), entry),
                                game_settings.cardpack_paths.end());
                        } else
                            game_settings.cardpack_paths.emplace_back(entry);
                    }
                }
                ImGui::EndChildFrame();
            }
        }

        if (ImGui::Button("Back"))
            game_settings_opened = false;

        ImGui::SameLine();
        if (ImGui::Button("OK")) {
            auto& game = wrapper->wrapper->game;

            { // Update players vector
                game.state.players.clear();

                for (int i = 0; i < game_settings.total_players; i++)
                    game.state.players.emplace_back(game.state, i);

                for (auto& player : game.state.players) {
                    player.hand_max_cards = game.state.default_hand_max_cards;
                }

                // Set local player name
                game.state.players[game.local_player_id].name = game_settings.local_player_name;
            }

            { // Update gamerules
                // @todo Gamerules are not updated
                // @body Gamerules are not updated when they change from init to when the OK button
                // is pressed
                game.state.active_coroutines.clear(); // Clear the active coroutines vector because
                                                      // it contains the old gamerules' game_flow
                game.gamerules = GameRules(game.state, game_settings.gamerules_path->string());
            }

            { // Add cardpacks
                for (auto& cardpack : game_settings.cardpack_paths)
                    game.state.add_cardpack(cardpack);

                std::cout << "Cards loaded: " << game.get_state().all_cards.size() << std::endl;
                wrapper->wrapper->renderer.game_renderer.update_sprite_vector();
            }

            { // Update AI players vector
                std::vector<PlayerPtr> players_to_control;

                for (int i = 1; i < game_settings.total_players; i++)
                    players_to_control.emplace_back(game.state.players[i]);

                wrapper->wrapper->ai_manager = AIManager(game.state, players_to_control,
                                                         game_settings.ai_path->generic_string());
            }

            game_settings_opened = false;
            ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
            sound::stop_music();
            return TitleScreenRenderer::Status::EnterGamePlaying;
        }

        if (!game_settings_opened)
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    return update_status(delta_time);
}

float TitleScreenRenderer::get_menu_option_y_offset(size_t opt_index) {
    return opt_index * text_spacing;
}

#pragma region Render Functions
void TitleScreenRenderer::render_background(float delta_time) {
    switch (status) {
        case (Status::LoadingAssets): break;

        case Status::ExitLoadingAssets: {
            // Linear transition between transparent and opaque
            float alpha = time_since_last_status_change / exit_loading_screen_transition_time;
            dynamic_bg.color.a = alpha;
            static_bg.color.a = alpha;

            dynamic_bg.render();
            dynamic_bg.update_scroll(delta_time);
            static_bg.render();
            break;
        }

        default:
            dynamic_bg.render();
            dynamic_bg.update_scroll(delta_time);
            static_bg.render();
            break;
    }
}

void TitleScreenRenderer::render_logo(float delta_time) {
    renderer::SpriteRenderer sprite_renderer;
    // Bind sprite shader
    glUseProgram(sprite_shader.get().handle);

    glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(wrapper->projection));

    const auto& logo_tex = logo_texture.get();
    const float window_w = wrapper->wrapper->game.window_w;
    const float window_h = wrapper->wrapper->game.window_h;
    sprite_renderer.set_camera_drag(false);
    sprite_renderer.set_scale({logo_tex.w / 2.f, logo_tex.h / 2.f});
    sprite_renderer.set_texture(logo_tex.handle);
    switch (status) {
        // Display at center if on loading screen
        case Status::LoadingAssets:
            sprite_renderer.set_position({window_w / 2.f, window_h / 2.f});
            break;

        // Transition smoothly between both states
        case Status::ExitLoadingAssets: {
            float lower_bound = window_h / 2.f;
            float upper_bound = window_h / 5.f * 4.f;
            float y_pos =
                lower_bound + (upper_bound - lower_bound) *
                                  sigmoid(time_since_last_status_change / logo_translation_time);
            sprite_renderer.set_position({window_w / 2.f, y_pos});
            break;
        }

        // Display at top if on any other screen
        default: sprite_renderer.set_position({window_w / 2.f, window_h / 5.f * 4.f}); break;
    }
    sprite_renderer.do_draw();
}

void TitleScreenRenderer::render_options(float delta_time) {
    renderer::FontRenderer renderer;
    float yoffset = 0.0f;

    renderer.set_size(text_scale);
    renderer.set_window_size(target->get_width(), target->get_height());

    for (auto& option : options) {
        std::string const& text = option.name;
        float alpha;
        switch (status) {
            case (Status::ExitLoadingAssets):
                alpha = sigmoid((time_since_last_status_change - options_appear_time) /
                                (exit_loading_screen_transition_time - options_appear_time)) *
                        option.color.a;
                break;

            default: alpha = option.color.a; break;
        }
        renderer.set_color({option.color.r, option.color.g, option.color.b, alpha});
        renderer.set_position(
            glm::vec2((-option.text_width / (float)target->get_width() / 2.f) * option.scale +
                          text_base_position.x,
                      yoffset + text_base_position.y));
        renderer.set_size(glm::vec2{1, 1} * option.scale);
        renderer.render_text(font, text);
        yoffset += text_spacing;
    }
}

void TitleScreenRenderer::render_loading_bar(float delta_time) {
    float alpha;
    switch(status) {
        case Status::LoadingAssets: alpha = 1; break;
        case Status::ExitLoadingAssets: alpha = 1.f - time_since_last_status_change / loading_bar_fade_time; break;
        default: return;
    }

    renderer::SpriteRenderer spr;

    auto& shader = solid_shader.get();
    glUseProgram(shader.handle);
    glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(wrapper->projection));

    const auto& window_w = wrapper->wrapper->game.window_w;
    const auto& window_h = wrapper->wrapper->game.window_h;
    // Draw white box as a border.
    spr.set_camera_drag(false);
    spr.set_color(1, 1, 1, alpha);
    spr.set_position({window_w / 2.f, window_h / 2.f - 150});
    spr.set_scale(glm::vec2(loading_bar_width, loading_bar_height));
    spr.do_draw();

    // Draw black box that fills the white box made previously.
    spr.set_color(0, 0, 0, alpha);
    spr.set_position({window_w / 2.f, window_h / 2.f - 150});
    spr.set_scale(glm::vec2(loading_bar_width - loading_bar_border_size * 2,
                            loading_bar_height - loading_bar_border_size * 2));
    spr.do_draw();

    // Draw white box that depicts loading progress.
    spr.set_color(1, 1, 1, alpha);
    spr.set_position({window_w / 2.f, window_h / 2.f - 150});
    spr.set_scale(glm::vec2((loading_bar_width - loading_bar_border_size * 2) *
                                loading_screen_data->load_progress(),
                            loading_bar_height - loading_bar_border_size * 2));
    spr.do_draw();
}
#pragma endregion

TitleScreenRenderer::Status TitleScreenRenderer::update_status(float delta_time) {
    for (size_t opt_index = 0; opt_index < options.size(); ++opt_index) {
        // Compute bounding box
        glm::vec2 top_left =
            text_base_position + glm::vec2(0, get_menu_option_y_offset(opt_index)) -
            glm::vec2(options[opt_index].text_width / target->get_width() / 2.f, 0);
        glm::vec2 bottom_right =
            top_left + glm::vec2(options[opt_index].text_width / target->get_width(),
                                 base_point_size / target->get_height());
        BoundingBox bbox{top_left, bottom_right};
        auto mouse_pos = input::get_mouse_pos();
        // Normalize mouse position
        mouse_pos.x /= target->get_width();
        mouse_pos.y /= target->get_height();
        if (inside_bounding_box(bbox, glm::vec2(mouse_pos.x, mouse_pos.y))) {
            auto& option = options[opt_index];
            option.color = glm::vec4(1, 1, 1, 1);
            option.scale += (selected_option_scale - option.scale) / offset_animate_slowness;
            if (!option.selected)
                audeo::play_sound(hover_sfx.get().source);
            option.selected = true;
            if (input::has_mousebutton_been_clicked(input::MouseButton::left)) {
                audeo::play_sound(click_sfx.get().source);
                status = options[opt_index].callback();
                return status;
            }
        } else {
            auto& option = options[opt_index];
            option.scale += (1 - option.scale) / offset_animate_slowness;
            option.color = default_option_color;
            option.selected = false;
        }
    }

    return Status::None;
}

void TitleScreenRenderer::add_main_menu_options() {
    options.clear();
    auto& fnt = font.get();
    options.emplace_back(
        "Local Game", [this]() { return local_game(); }, default_option_color,
        fnt.calculate_width("Local Game"));
    options.emplace_back(
        "Credits", [this]() { return credits(); }, default_option_color,
        fnt.calculate_width("Credits"));
    options.emplace_back(
        "Exit", [this]() { return quit(); }, default_option_color, fnt.calculate_width("Exit"));
}

void TitleScreenRenderer::show_loading_screen() {
    status = Status::LoadingAssets;
    time_since_last_status_change = 0;
    loading_screen_data =
        LoadingScreenData(assets::AssetManager::enumerate_assets("data/assets.json"));
    sound::stop_music();
    options.clear();
}

#pragma region Button Callbacks
TitleScreenRenderer::Status TitleScreenRenderer::quit() {
    return TitleScreenRenderer::Status::QuitApp;
}

TitleScreenRenderer::Status TitleScreenRenderer::local_game() {
    game_settings_opened = true;
    return TitleScreenRenderer::Status::None;
}

TitleScreenRenderer::Status TitleScreenRenderer::credits() {
    // Prepare option list for credits menu
    options.clear();
    options.emplace_back(
        "Back", [this]() { return exit_credits(); }, default_option_color,
        font.get().calculate_width("Back"));
    return TitleScreenRenderer::Status::Credits;
}

TitleScreenRenderer::Status TitleScreenRenderer::exit_credits() {
    // Prepare option list for main menu
    add_main_menu_options();
    return TitleScreenRenderer::Status::None;
}
#pragma endregion

} // namespace systems
} // namespace munchkin