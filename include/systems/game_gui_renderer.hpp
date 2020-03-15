//
// Created by aleok on 4/3/20.
//

#ifndef MUNCHKIN_ONLINE_GAME_GUI_RENDERER_HPP
#define MUNCHKIN_ONLINE_GAME_GUI_RENDERER_HPP

#include "renderer/assets.hpp"
#include <cstddef>
#include <sol/sol.hpp>
#include <vector>

namespace munchkin {
class RenderWrapper;

namespace systems {

class GameGUIRenderer;
class GameGUIRenderer {
public:
    GameGUIRenderer(RenderWrapper& wrapper);

    void render(float delta_time);

    struct ButtonWrapper;
    struct Button;
    struct Button {
        std::size_t id;
        std::string text;
        sol::function callback;
        sol::thread stack_thread;
        bool has_been_clicked = false;
        bool hovered = false;

        Button(){};
        ~Button() { std::cout << "Destroyed button." << std::endl; }

        Button(std::size_t id, std::string text, sol::function callback, sol::thread stack_thread) :
            id(id), text(text), callback(callback), stack_thread(stack_thread) {}

        Button(const Button& b) :
            id(b.id), text(b.text), callback(b.callback), stack_thread(b.stack_thread),
            hovered(b.hovered) {
            std::cout << "Copied (constructor) -> \t";
        }
        Button& operator=(const Button& b) {
            id = b.id;
            text = b.text;
            callback = b.callback;
            stack_thread = b.stack_thread;
            hovered = b.hovered;
            std::cout << "Copied (assignment) -> \t";
            return *this;
        }

        bool operator==(const Button& b) const { return b.id == id; }
    };

    struct ButtonWrapper {
        std::size_t id;

        ButtonWrapper() : id(-1){};
        explicit ButtonWrapper(std::nullptr_t) : id(-1){};
        explicit ButtonWrapper(Button& b) : id(b.id){};

        Button& get();

        std::string get_text() { return get().text; }
        void set_text(std::string const& str) { get().text = str; }

        sol::function get_callback() { return get().callback; }
        void set_callback(sol::function const& cb) { get().callback = cb; }

        bool get_hovered() { return get().hovered; }
        void set_hovered(bool b) { get().hovered = b; }

        bool operator==(ButtonWrapper const& b) const { return id == b.id; }
        bool exists() const;

        friend class GameGUIRenderer;
    };

    // API
    ButtonWrapper
    create_button(sol::this_state const&, std::string const& text, sol::function const& callback);
    void delete_button(ButtonWrapper button);

private:
    RenderWrapper* wrapper;
    std::unordered_map<std::size_t, Button> buttons;
    std::size_t last_button_id;

    assets::Handle<renderer::Shader> solid_shader;
    assets::Handle<renderer::Font> gui_font;
};

static GameGUIRenderer* game_gui_instance;

} // namespace systems
} // namespace munchkin

#endif // MUNCHKIN_ONLINE_GAME_GUI_RENDERER_HPP
