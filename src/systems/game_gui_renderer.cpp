//
// Created by aleok on 4/3/20.
//

#include "systems/game_gui_renderer.hpp"
#include "game_wrapper.hpp"
#include "input/input.hpp"
#include "render_wrapper.hpp"
#include "renderer/font_renderer.hpp"
#include "renderer/sprite_renderer.hpp"
#include <glm/glm/gtc/type_ptr.hpp>

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

    lua["gui"] = this;
    sol::usertype<GameGUIRenderer> gui_api = lua.new_usertype<GameGUIRenderer>("game_gui_renderer",
        "create_button", &GameGUIRenderer::create_button,
        "delete_button", &GameGUIRenderer::delete_button);
    /* clang-format on */

    auto& shader_manager = assets::get_manager<renderer::Shader>();
    auto& font_manager = assets::get_manager<renderer::Font>();
    solid_shader = shader_manager.load_asset(
        "solid_shader", {"data/shaders/solid.vert", "data/shaders/solid.frag"});
    gui_font = font_manager.load_asset("title_font", {"data/generic/quasimodo_regular.ttf"});
}

GameGUIRenderer::Button& GameGUIRenderer::ButtonWrapper::get() {
    return game_gui_instance->buttons.at(id);
}

bool GameGUIRenderer::ButtonWrapper::exists() const {
    return game_gui_instance->buttons.find(id) != game_gui_instance->buttons.end();
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

void GameGUIRenderer::delete_button(ButtonWrapper button) {
    buttons.erase(button.id);
}

void GameGUIRenderer::render(float delta_time) {
    auto& shader_manager = assets::get_manager<renderer::Shader>();

    renderer::SpriteRenderer spr;
    // Bind sprite shader
    auto& shader = shader_manager.get_asset(solid_shader);
    glUseProgram(shader.handle);

    glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(wrapper->projection));

    renderer::FontRenderer fnt;
    const auto window_w = wrapper->wrapper->game.window_w;
    const auto window_h = wrapper->wrapper->game.window_h;
    const auto& font = assets::get_manager<renderer::Font>().get_const_asset(gui_font);
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
        fnt.set_color((button.hovered ? 1.f : 0.8f) * glm::vec3{1, 1, 1});
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
}

} // namespace systems
} // namespace munchkin