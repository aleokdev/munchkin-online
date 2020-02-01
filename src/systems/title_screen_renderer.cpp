#include "systems/title_screen_renderer.hpp"

#include "input/input.hpp"
#include "renderer/assets.hpp"
#include "renderer/font_renderer.hpp"
#include "util/util.hpp"

#include <imgui.h>

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

TitleScreenRenderer::TitleScreenRenderer(::munchkin::GameWrapper& _wrapper) : wrapper(&_wrapper) {

    // Load assets

    auto& texture_manager = assets::get_manager<renderer::Texture>();
    auto& shader_manager = assets::get_manager<renderer::Shader>();
    auto& font_manager = assets::get_manager<renderer::Font>();

    assets::loaders::LoadParams<renderer::Texture> bg_params;
    bg_params.path = "data/generic/bg.png";
    background = renderer::create_background(texture_manager.load_asset("bg", bg_params));

    assets::loaders::LoadParams<renderer::Shader> sprite_shader_params{"data/shaders/sprite.vert",
                                                                       "data/shaders/sprite.frag"};
    sprite_shader = shader_manager.load_asset("sprite_shader", sprite_shader_params);

    assets::loaders::LoadParams<renderer::Font> font_params;
    font_params.path = "data/generic/quasimodo_regular.ttf";
    font = font_manager.load_asset("main_font", font_params);

    text_scale = glm::vec2(1, 1);
    text_base_position = glm::vec2(0.05f, 0.2f);

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
    renderer::render_background(background);

    if (status == Status::None) {
        render_menu_options();
    } else if (status == Status::Credits) {
        render_credits();
    }

    if (game_settings_opened) // OpenPopup needs to be used before the popup window and there can't
                              // be a new frame in between both
        ImGui::OpenPopup("Setup Game...");

    // TODO: Clean this up.
    if (ImGui::BeginPopupModal("Setup Game...", &game_settings_opened, ImGuiWindowFlags_AlwaysAutoResize)) {
        int total_players = wrapper->game.state.players.size();
        ImGui::InputInt("Total players", &total_players);
        total_players = std::max(0, total_players);
        if (total_players != wrapper->game.state.players.size()) {
            wrapper->game.state.players.clear();

            for (int i = 0; i < total_players; i++)
                wrapper->game.state.players.emplace_back(wrapper->game.state, i);

            for (auto& player : wrapper->game.state.players) {
                player.hand_max_cards = wrapper->game.state.default_hand_max_cards;
            }
        }
        int ai_players = wrapper->ai_manager.get_total_players_controlled();
        ImGui::InputInt("AI Players", &ai_players);
        ai_players = std::max(0, std::min(ai_players, total_players-1));
        if (ai_players != wrapper->ai_manager.get_total_players_controlled()) {
            std::vector<PlayerPtr> players_to_control;

            for (int i = 1; i < ai_players+1; i++)
                players_to_control.emplace_back(wrapper->game.state.players[i]);

            wrapper->ai_manager =
                AIManager(wrapper->game.state, players_to_control,
                          "data/ai/default"); // @todo: Don't hardcode default AI path
        }

        if (ImGui::Button("Back"))
            game_settings_opened = false;

        ImGui::SameLine();
        if (ImGui::Button("OK")) {
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