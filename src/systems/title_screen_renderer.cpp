#include "systems/title_screen_renderer.hpp"

#include "input/input.hpp"
#include "renderer/assets.hpp"
#include "renderer/font_renderer.hpp"
#include "util/util.hpp"

#include <audeo/audeo.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include <algorithm>

#include "game_wrapper.hpp"
#include "render_wrapper.hpp"

namespace munchkin {
namespace systems {

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
    auto& fnt = assets::get_manager<renderer::Font>().get_asset(font);
    options.emplace_back(
        "Back", [this]() { return exit_credits(); }, default_option_color,
        fnt.calculate_width("Back"));
    return TitleScreenRenderer::Status::Credits;
}

TitleScreenRenderer::Status TitleScreenRenderer::exit_credits() {
    // Prepare option list for main menu
    options.clear();
    auto& fnt = assets::get_manager<renderer::Font>().get_asset(font);
    options.emplace_back(
        "Local Game", [this]() { return local_game(); }, default_option_color,
        fnt.calculate_width("Local Game"));
    options.emplace_back(
        "Credits", [this]() { return credits(); }, default_option_color,
        fnt.calculate_width("Credits"));
    options.emplace_back(
        "Exit", [this]() { return quit(); }, default_option_color, fnt.calculate_width("Exit"));
    return TitleScreenRenderer::Status::None;
}

TitleScreenRenderer::TitleScreenRenderer(::munchkin::RenderWrapper& _wrapper) :
    wrapper(&_wrapper), static_bg(assets::get_manager<renderer::Texture>().load_asset(
                            "vignette", {"data/generic/vignette.png"})),
    dynamic_bg(
        assets::get_manager<renderer::Texture>().load_asset("noise", {"data/generic/noise.png"}),
        true) {

    // Load assets

    auto& shader_manager = assets::get_manager<renderer::Shader>();
    auto& texture_manager = assets::get_manager<renderer::Texture>();
    auto& font_manager = assets::get_manager<renderer::Font>();
    auto& music_manager = assets::get_manager<assets::Music>();
    auto& sfx_manager = assets::get_manager<assets::SoundEffect>();

    assets::loaders::LoadParams<renderer::Shader> sprite_shader_params{"data/shaders/sprite.vert",
                                                                       "data/shaders/sprite.frag"};
    sprite_shader = shader_manager.load_asset("sprite_shader", sprite_shader_params);
    logo_texture = texture_manager.load_asset("logo", {"data/generic/logo.png"});

    hover_sfx = sfx_manager.load_asset("ui_hover", {"data/generic/ui_hover.wav"});
    click_sfx = sfx_manager.load_asset("ui_click", {"data/generic/ui_click.wav"});

    assets::loaders::LoadParams<renderer::Font> font_params;
    font_params.path = "data/generic/quasimodo_regular.ttf";
    font = font_manager.load_asset("main_font", font_params);

    text_scale = glm::vec2(1, 1);
    text_base_position = glm::vec2(0.5f, 0.4f);

    // Start the title music
    music = audeo::play_sound(
        music_manager
            .get_asset(music_manager.load_asset("title_song", {"data/generic/title_song.mp3"}))
            .source,
        audeo::loop_forever);

    // We're in the main menu state by default
    auto& fnt = assets::get_manager<renderer::Font>().get_asset(font);
    options.emplace_back(
        "Local Game", [this]() { return local_game(); }, default_option_color,
        fnt.calculate_width("Local Game"));
    options.emplace_back(
        "Credits", [this]() { return credits(); }, default_option_color,
        fnt.calculate_width("Credits"));
    options.emplace_back(
        "Exit", [this]() { return quit(); }, default_option_color, fnt.calculate_width("Exit"));
}

void TitleScreenRenderer::set_render_target(renderer::RenderTarget* tg) {
    target = tg;
    text_spacing = (text_scale.y * base_point_size + spacing) / target->get_height();
}

TitleScreenRenderer::Status TitleScreenRenderer::frame(float delta_time) {
    dynamic_bg.render();
    dynamic_bg.update_scroll(delta_time);
    static_bg.render();

    renderer::SpriteRenderer sprite_renderer;
    // Bind sprite shader
    glUseProgram(assets::get_manager<renderer::Shader>().get_asset(sprite_shader).handle);

    glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(wrapper->projection));

    const auto& logo_tex = assets::get_manager<renderer::Texture>().get_asset(logo_texture);
    const float window_w = wrapper->wrapper->game.window_w;
    const float window_h = wrapper->wrapper->game.window_h;
    sprite_renderer.set_camera_drag(false);
    sprite_renderer.set_position({window_w / 2.f, window_h / 5.f * 4.f});
    sprite_renderer.set_scale({logo_tex.w / 2.f, logo_tex.h / 2.f});
    sprite_renderer.set_texture(logo_tex.handle);

    sprite_renderer.do_draw();

    if (status == Status::None) {
        render_menu_options();
    } else if (status == Status::Credits) {
        render_credits();
    }

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

        { // AI players slider
            int ai_players = game_settings.total_ai_players;
            ImGui::InputInt("AI Players", &ai_players);
            game_settings.total_ai_players =
                std::max(0, std::min(ai_players, (int)game_settings.total_players - 1));
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

                for (int i = 1; i < game_settings.total_ai_players + 1; i++)
                    players_to_control.emplace_back(game.state.players[i]);

                wrapper->wrapper->ai_manager = AIManager(game.state, players_to_control,
                                                         game_settings.ai_path->generic_string());
            }

            game_settings_opened = false;
            ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
            audeo::stop_sound(music, 200);
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

void TitleScreenRenderer::render_menu_options() {
    renderer::FontRenderer renderer;
    float yoffset = 0.0f;

    renderer.set_size(text_scale);
    renderer.set_window_size(target->get_width(), target->get_height());

    for (auto& option : options) {
        std::string const& text = option.name;
        renderer.set_color(option.color);
        renderer.set_position(
            glm::vec2((-option.text_width / (float)target->get_width() / 2.f) * option.scale +
                          text_base_position.x,
                      yoffset + text_base_position.y));
        renderer.set_size(glm::vec2{1, 1} * option.scale);
        renderer.render_text(font, text);
        yoffset += text_spacing;
    }
}
void TitleScreenRenderer::render_credits() {
    renderer::FontRenderer renderer;
    float yoffset = 0.0f;

    renderer.set_size(text_scale);
    renderer.set_window_size(target->get_width(), target->get_height());

    for (auto& option : options) {
        std::string const& text = option.name;
        renderer.set_color(option.color);
        renderer.set_position(
            glm::vec2((-option.text_width / (float)target->get_width() / 2.f) * option.scale +
                          text_base_position.x,
                      yoffset + text_base_position.y));
        renderer.set_size(glm::vec2{1, 1} * option.scale);
        renderer.render_text(font, text);
        yoffset += text_spacing;
    }
}

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
            option.color = glm::vec3(1, 1, 1);
            option.scale += (selected_option_scale - option.scale) / offset_animate_slowness;
            if (!option.selected)
                audeo::play_sound(
                    assets::get_manager<assets::SoundEffect>().get_asset(hover_sfx).source);
            option.selected = true;
            if (input::has_mousebutton_been_clicked(input::MouseButton::left)) {
                audeo::play_sound(
                    assets::get_manager<assets::SoundEffect>().get_asset(click_sfx).source);
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

} // namespace systems
} // namespace munchkin