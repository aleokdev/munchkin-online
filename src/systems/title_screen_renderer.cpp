#include "systems/title_screen_renderer.hpp"

#include "input/input.hpp"
#include "renderer/assets.hpp"
#include "renderer/font_renderer.hpp"
#include "util/util.hpp"

#include <audeo/audeo.hpp>
#include <imgui.h>

#include <algorithm>

#include "game_wrapper.hpp"

namespace munchkin {
namespace systems {

namespace option_callbacks {

TitleScreenRenderer::Status quit(TitleScreenRenderer&);
TitleScreenRenderer::Status local_game(TitleScreenRenderer&);
TitleScreenRenderer::Status credits(TitleScreenRenderer& tr);
TitleScreenRenderer::Status exit_credits(TitleScreenRenderer& tr);

TitleScreenRenderer::Status quit(TitleScreenRenderer&) {
    return TitleScreenRenderer::Status::QuitApp;
}

TitleScreenRenderer::Status local_game(TitleScreenRenderer& tr) {
    tr.game_settings_opened = true;
    return TitleScreenRenderer::Status::None;
}

TitleScreenRenderer::Status credits(TitleScreenRenderer& tr) {
    // Prepare option list for credits menu
    tr.options.clear();
    tr.options.push_back({"Back", option_callbacks::exit_credits, tr.default_option_color});
    return TitleScreenRenderer::Status::Credits;
}

TitleScreenRenderer::Status exit_credits(TitleScreenRenderer& tr) {
    // Prepare option list for main menu
    tr.options.clear();
    tr.options.push_back({"Local Game", option_callbacks::local_game, tr.default_option_color});
    tr.options.push_back({"Credits", option_callbacks::credits, tr.default_option_color});
    tr.options.push_back({"Exit", option_callbacks::quit, tr.default_option_color});
    return TitleScreenRenderer::Status::None;
}

} // namespace option_callbacks

TitleScreenRenderer::TitleScreenRenderer(::munchkin::GameWrapper& _wrapper) :
    wrapper(&_wrapper), static_bg(assets::get_manager<renderer::Texture>().load_asset(
                            "vignette", {"data/generic/vignette.png"})),
    dynamic_bg(
        assets::get_manager<renderer::Texture>().load_asset("noise", {"data/generic/noise.png"}),
        true) {

    // Load assets

    auto& shader_manager = assets::get_manager<renderer::Shader>();
    auto& font_manager = assets::get_manager<renderer::Font>();
    auto& music_manager = assets::get_manager<assets::Music>();

    assets::loaders::LoadParams<renderer::Shader> sprite_shader_params{"data/shaders/sprite.vert",
                                                                       "data/shaders/sprite.frag"};
    sprite_shader = shader_manager.load_asset("sprite_shader", sprite_shader_params);

    assets::loaders::LoadParams<renderer::Font> font_params;
    font_params.path = "data/generic/quasimodo_regular.ttf";
    font = font_manager.load_asset("main_font", font_params);

    text_scale = glm::vec2(1, 1);
    text_base_position = glm::vec2(0.05f, 0.2f);

    // Start the title music
    audeo::play_sound(
        music_manager.get_asset(music_manager.load_asset("title_music", {"data/generic/title.mp3"}))
            .source,
        audeo::loop_forever);

    // We're in the main menu state by default
    options.push_back({"Local Game", option_callbacks::local_game, default_option_color});
    options.push_back({"Credits", option_callbacks::credits, default_option_color});
    options.push_back({"Exit", option_callbacks::quit, default_option_color});
}

void TitleScreenRenderer::set_render_target(renderer::RenderTarget* tg) {
    target = tg;
    text_spacing = (text_scale.y * base_point_size + spacing) / target->get_height();
}

TitleScreenRenderer::Status TitleScreenRenderer::frame(float delta_time) {
    dynamic_bg.render();
    dynamic_bg.update_scroll(delta_time);
    static_bg.render();

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

            { // Update players vector
                wrapper->game.state.players.clear();

                for (int i = 0; i < game_settings.total_players; i++)
                    wrapper->game.state.players.emplace_back(wrapper->game.state, i);

                for (auto& player : wrapper->game.state.players) {
                    player.hand_max_cards = wrapper->game.state.default_hand_max_cards;
                }

                // Set local player name
                wrapper->game.state.players[wrapper->game.local_player_id].name =
                    game_settings.local_player_name;
            }

            { // Update gamerules
                // @todo Gamerules are not updated
                // @body Gamerules are not updated when they change from init to when the OK button
                // is pressed
                wrapper->game.state.active_coroutines
                    .clear(); // Clear the active coroutines vector because it contains the old
                              // gamerules' game_flow
                wrapper->game.gamerules =
                    GameRules(wrapper->game.state, game_settings.gamerules_path->string());
            }

            { // Add cardpacks
                for (auto& cardpack : game_settings.cardpack_paths)
                    wrapper->game.state.add_cardpack(cardpack);

                std::cout << "Cards loaded: " << wrapper->game.get_state().all_cards.size()
                          << std::endl;
                wrapper->renderer.game_renderer.update_sprite_vector();
            }

            { // Update AI players vector
                std::vector<PlayerPtr> players_to_control;

                for (int i = 1; i < game_settings.total_ai_players + 1; i++)
                    players_to_control.emplace_back(wrapper->game.state.players[i]);

                wrapper->ai_manager = AIManager(wrapper->game.state, players_to_control,
                                                game_settings.ai_path->generic_string());
            }

            game_settings_opened = false;
            ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
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

float TitleScreenRenderer::calculate_text_width(std::string const& text) {
    return (text.size() * text_scale.x * base_point_size) / target->get_width();
}

void TitleScreenRenderer::render_menu_options() {
    renderer::FontRenderer renderer;
    float yoffset = 0.0f;

    renderer.set_size(text_scale);
    renderer.set_window_size(target->get_width(), target->get_height());

    auto render_option = [this, &renderer, &yoffset](size_t i) {
        std::string const& text = options[i].name;
        renderer.set_color(options[i].color);
        float xoffset = options[i].offset / target->get_width();
        renderer.set_position(
            glm::vec2(text_base_position.x + xoffset, text_base_position.y + yoffset));
        renderer.render_text(font, text);
        yoffset += text_spacing;
    };

    for (size_t i = 0; i < options.size(); ++i) { render_option(i); }
}
void TitleScreenRenderer::render_credits() {
    renderer::FontRenderer renderer;
    float yoffset = 0.0f;

    renderer.set_size(text_scale);
    renderer.set_window_size(target->get_width(), target->get_height());

    auto render_option = [this, &renderer, &yoffset](size_t i) {
        std::string const& text = options[i].name;
        renderer.set_color(options[i].color);
        float xoffset = options[i].offset / target->get_width();
        renderer.set_position(
            glm::vec2(text_base_position.x + xoffset, text_base_position.y + yoffset));
        renderer.render_text(font, text);
        yoffset += text_spacing;
    };

    for (size_t i = 0; i < options.size(); ++i) { render_option(i); }
}

TitleScreenRenderer::Status TitleScreenRenderer::update_status(float delta_time) {
    for (size_t opt_index = 0; opt_index < options.size(); ++opt_index) {
        // Compute bounding box
        glm::vec2 top_left = text_base_position + glm::vec2(0, get_menu_option_y_offset(opt_index));
        glm::vec2 bottom_right = top_left + glm::vec2(calculate_text_width(options[opt_index].name),
                                                      base_point_size / target->get_height());
        BoundingBox bbox{top_left, bottom_right};
        auto mouse_pos = input::get_mouse_pos();
        // Normalize mouse position
        mouse_pos.x /= target->get_width();
        mouse_pos.y /= target->get_height();
        if (inside_bounding_box(bbox, glm::vec2(mouse_pos.x, mouse_pos.y))) {
            auto& option = options[opt_index];
            option.color = glm::vec3(1, 1, 1);
            option.offset += (selected_option_offset - option.offset) / offset_animate_slowness;
            if (input::has_mousebutton_been_clicked(input::MouseButton::left)) {
                status = options[opt_index].callback(*this);
                return status;
            }
        } else {
            auto& option = options[opt_index];
            option.offset += (0 - option.offset) / offset_animate_slowness;
            option.color = default_option_color;
        }
    }

    return Status::None;
}

} // namespace systems
} // namespace munchkin