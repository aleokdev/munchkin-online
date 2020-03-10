//
// Created by aleok on 4/3/20.
//

#ifndef MUNCHKIN_ONLINE_GAME_GUI_RENDERER_HPP
#define MUNCHKIN_ONLINE_GAME_GUI_RENDERER_HPP

#include <cstddef>
#include <vector>
#include <sol/sol.hpp>
#include "renderer/assets.hpp"

namespace munchkin {
class RenderWrapper;

namespace systems {

class GameGUIRenderer;
class GameGUIRenderer {
public:
    GameGUIRenderer(RenderWrapper& wrapper);

    void render(float delta_time);

    struct ButtonPtr;
    struct Button {
        std::size_t id;
        std::string text;
        sol::function callback;
        bool hovered = false;

        ButtonPtr ptr();

        bool operator==(const Button& b) const{
            return b.id == id;
        }
    };

    struct ButtonPtr {
        std::size_t id;

        ButtonPtr() : id(-1){};
        explicit ButtonPtr(std::nullptr_t) : id(-1) {};
        explicit ButtonPtr(Button& b) : id(b.id) {};
        explicit ButtonPtr(std::size_t _id) : id(_id){};
        ButtonPtr(ButtonPtr&) = default;
        ButtonPtr(ButtonPtr const&) = default;
        ButtonPtr& operator=(ButtonPtr const&) = default;
        ButtonPtr& operator=(ButtonPtr&&) = default;
        ButtonPtr& operator=(std::nullptr_t) {
            id = -1;
            return *this;
        }

        void reset() { id = -1; }

        Button* operator->() const;
        Button& operator*() const { return *this->operator->(); }
        [[nodiscard]] Button* get() const { return this->operator->(); }

        operator bool() const { return id == -1; }
        bool operator==(ButtonPtr const& b) const { return id == b.id; }
        bool operator==(std::nullptr_t) const { return id == -1; }
    };

    // API
    ButtonPtr create_button(std::string const& text, sol::function const& callback);
    void delete_button(ButtonPtr& button);

private:
    RenderWrapper* wrapper;
    std::unordered_map<std::size_t, Button> buttons;
    std::size_t last_button_id;

    assets::Handle<renderer::Shader> solid_shader;
    assets::Handle<renderer::Font> gui_font;
};

static GameGUIRenderer* game_gui_instance;



}
}

namespace sol {
template<> struct unique_usertype_traits<munchkin::systems::GameGUIRenderer::ButtonPtr> {
    typedef munchkin::systems::GameGUIRenderer::Button type;
    typedef munchkin::systems::GameGUIRenderer::ButtonPtr actual_type;
    static const bool value = true;

    static bool is_null(const actual_type& ptr) { return ptr == nullptr; }
    static type* get(const actual_type& ptr) { return ptr.get(); }
};
} // namespace sol

#endif // MUNCHKIN_ONLINE_GAME_GUI_RENDERER_HPP
