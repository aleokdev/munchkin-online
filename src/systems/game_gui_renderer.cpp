#include "systems/game_gui_renderer.hpp"
#include "game_wrapper.hpp"
#include "input/input.hpp"
#include "render_wrapper.hpp"
#include "renderer/font_renderer.hpp"

namespace munchkin {
namespace systems {

GameGUIRenderer::GameGUIRenderer(RenderWrapper& w) : wrapper(&w) {
    game_gui_instance = this;

    auto& lua = w.wrapper->game.state.lua;
    /* clang-format off */
    lua.new_usertype<ButtonWrapper>("game_gui_renderer_button",
        "text", sol::property(&ButtonWrapper::get_text, &ButtonWrapper::set_text),
        "callback", sol::property(&ButtonWrapper::get_callback, &ButtonWrapper::set_callback),
        "exists", &ButtonWrapper::exists);

    lua.new_usertype<LabelWrapper>("game_gui_renderer_label",
        "text", sol::property(&LabelWrapper::get_text, &LabelWrapper::set_text),
        "color", sol::property(&LabelWrapper::get_color, &LabelWrapper::set_color),
        "anchor", sol::property(&LabelWrapper::get_anchor, &LabelWrapper::set_anchor),
        "horizontal_alignment", sol::property(&LabelWrapper::get_horizontal_alignment, &LabelWrapper::set_horizontal_alignment),
        "pos", sol::property(&LabelWrapper::get_pos, &LabelWrapper::set_pos),
        "exists", &LabelWrapper::exists);

    lua.new_enum<GUIAnchor>("gui_anchor", {
        {"upper_left_corner", GUIAnchor::upper_left_corner},
        {"upper_right_corner", GUIAnchor::upper_right_corner},
        {"lower_left_corner", GUIAnchor::lower_left_corner},
        {"lower_right_corner", GUIAnchor::lower_right_corner},
        {"center", GUIAnchor::center}});

    lua.new_enum<TextAlignment>("text_alignment", {
        {"start_at_pos", TextAlignment::start_at_pos},
        {"center", TextAlignment::center},
        {"end_at_pos", TextAlignment::end_at_pos}
});

    lua["gui"] = this;
    sol::usertype<GameGUIRenderer> gui_api = lua.new_usertype<GameGUIRenderer>("game_gui_renderer",
        "create_button", &GameGUIRenderer::create_button,
        "delete_button", &GameGUIRenderer::delete_button,
        "create_label", &GameGUIRenderer::create_label,
        "delete_label", &GameGUIRenderer::delete_label);
    /* clang-format on */

    solid_shader = assets::AssetManager::load_asset<renderer::Shader>("solid_shader");
    gui_font = assets::AssetManager::load_asset<renderer::Font>("title_font");
}

GameGUIRenderer::Button& GameGUIRenderer::ButtonWrapper::get() {
    return game_gui_instance->buttons.at(id);
}

bool GameGUIRenderer::ButtonWrapper::exists() const {
    return game_gui_instance->buttons.find(id) != game_gui_instance->buttons.end();
}

GameGUIRenderer::Label& GameGUIRenderer::LabelWrapper::get() {
    return game_gui_instance->labels.at(id);
}

bool GameGUIRenderer::LabelWrapper::exists() const {
    return game_gui_instance->labels.find(id) != game_gui_instance->labels.end();
}

GameGUIRenderer::ButtonWrapper GameGUIRenderer::create_button(sol::this_state const& s,
                                                              std::string const& text,
                                                              sol::function const& callback) {
    sol::thread thread(sol::thread::create(s.L));
    sol::function f(thread.thread_state(), callback);
    auto id_to_use = last_button_id++;
    auto button = buttons.emplace(id_to_use, Button(id_to_use, text, f, thread));
    return ButtonWrapper(button.first->second);
}

void GameGUIRenderer::delete_button(ButtonWrapper button) { buttons.erase(button.id); }

GameGUIRenderer::LabelWrapper GameGUIRenderer::create_label(sol::this_state const& s,
                                                            std::string const& text) {
    auto id_to_use = last_label_id++;
    auto label = Label();
    label.id = id_to_use;
    label.text = text;
    labels.emplace(id_to_use, label);
    return LabelWrapper(label);
}

void GameGUIRenderer::delete_label(LabelWrapper label) { labels.erase(label.id); }

void GameGUIRenderer::render(float delta_time) {
    renderer::FontRenderer fnt;
    const auto window_w = wrapper->wrapper->game.window_w;
    const auto window_h = wrapper->wrapper->game.window_h;
    const auto& font = gui_font.get();
    fnt.set_window_size(window_w, window_h);

    int index = 1;
    Button* do_callback = nullptr;
    for (auto& [_, button] : buttons) {
        glm::vec2 btn_pos{0.f, 1 - (float)index / 10.f};
        glm::vec2 btn_pixel_pos = btn_pos * glm::vec2{window_w, window_h};
        math::Rect2D btn_pixel_rect{math::Vec2D{btn_pixel_pos.x, btn_pixel_pos.y},
                                    math::Vec2D{font.calculate_width(button.text), 48.f}};
        button.hovered = btn_pixel_rect.contains(input::get_mouse_pos());
        fnt.set_position(btn_pos);
        fnt.set_color({1, 1, 1, button.hovered ? 1.f : 0.8f});
        fnt.render_text(gui_font, button.text);

        if (button.hovered && input::is_mousebutton_pressed(input::MouseButton::left)) {
            if (!button.has_been_clicked) {
                button.has_been_clicked = true;
                do_callback = &button;
            }
        } else
            button.has_been_clicked = false;
        index++;
    }

    // Use a pointer for doing the callback because it could erase the button from the
    // list while iterating
    if (do_callback) {
        sol::coroutine coro(do_callback->callback);
        coro(ButtonWrapper(*do_callback));
        wrapper->wrapper->game.state.active_coroutines.emplace_back(coro);
    }

    for (auto& [_, label] : labels) {
        // NDC coordinates
        float x_alignment_offset;
        switch (label.horizontal_alignment) {
            case TextAlignment::start_at_pos: x_alignment_offset = 0; break;
            case TextAlignment::center:
                x_alignment_offset = -font.calculate_width(label.text) / (float)window_w / 2.f;
                break;
            case TextAlignment::end_at_pos:
                x_alignment_offset = -font.calculate_width(label.text) / (float)window_w;
                break;
        }
        glm::vec2 anchor_pos;
        switch (label.anchor) {
            case GUIAnchor::upper_left_corner: anchor_pos = {0, 0}; break;
            case GUIAnchor::upper_right_corner: anchor_pos = {1, 0}; break;
            case GUIAnchor::lower_left_corner: anchor_pos = {0, 1}; break;
            case GUIAnchor::lower_right_corner: anchor_pos = {1, 1}; break;
            case GUIAnchor::center: anchor_pos = {.5f, .5f}; break;
        }
        glm::vec2 pos = anchor_pos + glm::vec2{x_alignment_offset, 0} + label.pos;
        fnt.set_position(pos);
        fnt.set_color(label.color);
        fnt.render_text(gui_font, label.text);
    }
}

} // namespace systems
} // namespace munchkin