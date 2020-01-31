#ifndef MUNCHKIN_FONT_RENDERER_HPP__
#define MUNCHKIN_FONT_RENDERER_HPP__

#include "renderer/assets.hpp"

#include <glm/glm.hpp>
#include <string>

namespace munchkin {
namespace renderer {

class FontRenderer {
public:
    FontRenderer();

    void set_window_size(size_t w, size_t h);
    void set_size(glm::vec2 size);
    void set_position(glm::vec2 position);
    void set_color(glm::vec3 color);

    void render_char(Font::glyph_data const& data,
                    glm::vec2 position,
                    glm::vec2 size,
                    glm::vec2 offset);
    void render_text(assets::Handle<Font> font, std::string const& text);

    static void deallocate();

private:
    // shared data for vertices
    static inline unsigned int vao = 0, vbo = 0, ebo = 0;

    static inline assets::Handle<Shader> shader;

    static void init();

    static void setup_for_render();

    glm::vec2 text_size = glm::vec2(1.0f, 1.0f);
    glm::vec2 text_position;
    glm::vec3 text_color = glm::vec3(1, 1, 1);
    size_t window_w = 0;
    size_t window_h = 0;
};

} // namespace renderer
} // namespace munchkin

#endif