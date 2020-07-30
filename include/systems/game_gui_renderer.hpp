//
// Created by aleok on 4/3/20.
//

#ifndef MUNCHKIN_ONLINE_GAME_GUI_RENDERER_HPP
#define MUNCHKIN_ONLINE_GAME_GUI_RENDERER_HPP

#include "assets/assets.hpp"
#include <cstddef>
#include <set>
#include <sol/sol.hpp>
#include <stack>
#include <vector>

namespace munchkin {
class RenderWrapper;
struct CardPtr;

namespace systems {

class GameGUIRenderer;
class [[system_codegen::renderer_instance("game_gui_renderer")]] GameGUIRenderer {
public:
    GameGUIRenderer(RenderWrapper& wrapper);

    void load_content();

    void render(float delta_time);

    enum class GUIAnchor {
        upper_left_corner,
        upper_right_corner,
        lower_left_corner,
        lower_right_corner,
        center
    };
    enum class TextAlignment {
        // Text starts on its position, left-to-right.
        start_at_pos,
        // Middle of the bounds of the text is equal to its position.
        center,
        // Text ends on its position, left-to-right.
        end_at_pos
    };

    struct ButtonWrapper;
    struct Button;
    struct Button {
        std::size_t id;
        std::string text;
        sol::function callback;
        sol::thread stack_thread;
        bool has_been_clicked = false;
        bool hovered = false;
        // TODO: Add position

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

        bool operator==(Button const& b) const { return b.id == id; }
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
        [[nodiscard]] bool exists() const;

        friend class GameGUIRenderer;
    };

    struct LabelWrapper;
    struct Label;
    struct Label {
        std::size_t id;
        std::string text;
        glm::vec4 color = {1, 1, 1, 1};
        GUIAnchor anchor = GUIAnchor::upper_left_corner;
        TextAlignment horizontal_alignment = TextAlignment::start_at_pos;
        // Position of the text relative to its anchor.
        // Uses normalized coordinates between 0 (left, top) and 1 (right, bottom)
        glm::vec2 pos;

        bool operator==(Label const& other) const { return id == other.id; }
    };

    struct LabelWrapper {
        std::size_t id;

        LabelWrapper() : id(-1){};
        explicit LabelWrapper(std::nullptr_t) : id(-1){};
        explicit LabelWrapper(Label& l) : id(l.id){};

        Label& get();

        std::string get_text() { return get().text; }
        void set_text(std::string const& str) { get().text = str; }

        glm::vec4 get_color() { return get().color; }
        void set_color(glm::vec4 const& c) { get().color = c; }

        GUIAnchor get_anchor() { return get().anchor; }
        void set_anchor(GUIAnchor anchor) { get().anchor = anchor; }

        TextAlignment get_horizontal_alignment() { return get().horizontal_alignment; }
        void set_horizontal_alignment(TextAlignment align) { get().horizontal_alignment = align; }

        glm::vec2 get_pos() { return get().pos; }
        void set_pos(glm::vec2 const& pos) { get().pos = pos; }

        bool operator==(LabelWrapper const& b) const { return id == b.id; }
        [[nodiscard]] bool exists() const;

        friend class GameGUIRenderer;
    };

    // API
    ButtonWrapper
    create_button(sol::this_state const&, std::string const& text, sol::function const& callback);
    void delete_button(ButtonWrapper button);
    LabelWrapper create_label(sol::this_state const&, std::string const& text);
    void delete_label(LabelWrapper label);

private:
    RenderWrapper* wrapper;
    std::unordered_map<std::size_t, Button> buttons;
    std::unordered_map<std::size_t, Label> labels;
    std::size_t last_button_id;
    std::size_t last_label_id;

    assets::Handle<renderer::Shader> solid_shader;
    assets::Handle<renderer::Font> gui_font;
};

static GameGUIRenderer* game_gui_instance;

} // namespace systems
} // namespace munchkin

#endif // MUNCHKIN_ONLINE_GAME_GUI_RENDERER_HPP
