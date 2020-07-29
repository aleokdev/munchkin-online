#include "renderer/font_renderer.hpp"
#include "renderer/font.hpp"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace munchkin {
namespace renderer {

FontRenderer::FontRenderer() { setup_for_render(); }

void FontRenderer::deallocate() {
    if (vao) {
        glDeleteVertexArrays(1, &vao);
    }
    if (vbo) {
        glDeleteBuffers(1, &vbo);
    }
    if (ebo) {
        glDeleteBuffers(1, &ebo);
    }
}

void FontRenderer::setup_for_render() {
    if (!vao || !vbo || !ebo) {
        deallocate();
        init();
    }

    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
}

void FontRenderer::init() {
    static float vertices[] = {
        // Vertices	        Texture coords
        -1.0f, 1.0f,  0.0f, 1.0f, // TL
        -1.0f, -1.0f, 0.0f, 0.0f, // BL
        1.0f,  -1.0f, 1.0f, 0.0f, // BR
        1.0f,  1.0f,  1.0f, 1.0f, // TR
    };

    static unsigned int elements[] = {0, 1, 2, 0, 2, 3};

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    // Fill buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    // Setup vao
    glBindVertexArray(vao);

    // Positions
    glEnableVertexAttribArray(0);
    glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, 0);
    glBindVertexBuffer(0, vbo, 0, 4 * sizeof(float));
    glVertexAttribBinding(0, 0);

    // TexCoords
    glEnableVertexAttribArray(1);
    glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float));
    glBindVertexBuffer(1, vbo, 0, 4 * sizeof(float));
    glVertexAttribBinding(1, 1);

    shader = assets::AssetManager::load_asset<Shader>("text_shader");
}

void FontRenderer::set_window_size(size_t w, size_t h) {
    window_w = w;
    window_h = h;
}

void FontRenderer::set_size(glm::vec2 size) { text_size = size; }

void FontRenderer::set_position(glm::vec2 position) { text_position = position; }

void FontRenderer::set_color(glm::vec4 color) { text_color = color; }

void FontRenderer::set_shader(assets::Handle<Shader> sh) {
    opt_shader = sh;
}

void FontRenderer::render_char(Font::glyph_data const& data,
                               glm::vec2 position,
                               glm::vec2 size,
                               glm::vec2 offset) {

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, data.texture);
    glUniform1i(5, 0);
    glm::vec2 img_dim = glm::vec2(data.size.x, data.size.y);
    glm::vec2 target_dim = glm::vec2(window_w, window_h);
    glm::vec2 scale =
        glm::vec2(img_dim.x / target_dim.x * size.x, img_dim.y / target_dim.y * size.y);
    glUniform2fv(0, 1, glm::value_ptr(scale));
    glUniform2fv(1, 1, glm::value_ptr(position));
    glUniform2fv(2, 1, glm::value_ptr(offset));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void FontRenderer::render_text(assets::Handle<Font> font, std::string const& text) {
    // If the optional shader was set, use that one. Otherwise, use default shader
    auto& shader_program = (opt_shader ? opt_shader : shader).get();
    auto& font_data = font.get();
    glUseProgram(shader_program.handle);

    glUniform4fv(3, 1, glm::value_ptr(text_color));

    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);

    glm::vec2 offset = glm::vec2(0, 0);
    for (auto const& c : text) {
        if (c == 0)
            break;
        Font::glyph_data const& data = font_data.glyphs[c];
        if (c == '\n') {
            offset.y += data.pixel_size * text_size.y;
            offset.x = 0;
        }
        else if (offset.x == 0 && c == ' ') // Skip spaces on new lines
            continue;

        glm::vec2 offset_pos =
            offset + glm::vec2(data.bearing.x, data.pixel_size - data.bearing.y) * text_size;
        glm::vec2 relative_offset =
            glm::vec2(offset_pos.x / (float)window_w, offset_pos.y / (float)window_h);
        render_char(data, text_position, text_size, relative_offset);
        // shift by 6 to get the offset in pixels
        offset.x += (data.advance >> 6) * text_size.x;
    }
}

void FontRenderer::render_wrapped_text(assets::Handle<Font> font,
                                       float max_text_width,
                                       std::string const& text) {
    // If the optional shader was set, use that one. Otherwise, use default shader
    auto& shader_program = (opt_shader.id ? opt_shader : shader).get();
    auto& font_data = font.get();
    glUseProgram(shader_program.handle);

    glUniform3fv(3, 1, glm::value_ptr(text_color));

    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);

    glm::vec2 offset = glm::vec2(0, 0);
    for (auto const& c : text) {
        if (c == 0)
            break;
        Font::glyph_data const& data = font_data.glyphs[c];
        if(c == '\n')
        {
            offset.y += data.pixel_size * text_size.y;
            offset.x = 0;
        }
        else if (offset.x == 0 && c == ' ') // Skip spaces on new lines
            continue;

        glm::vec2 offset_pos =
            offset + glm::vec2(data.bearing.x, data.pixel_size - data.bearing.y) * text_size;
        glm::vec2 relative_offset =
            glm::vec2(offset_pos.x / (float)window_w, offset_pos.y / (float)window_h);

        render_char(data, text_position, text_size, relative_offset);
        // shift by 6 to get the offset in pixels
        offset.x += (data.advance >> 6) * text_size.x;
        if(relative_offset.x > max_text_width)
        {
            offset.y += data.pixel_size * text_size.y;
            offset.x = 0;
        }
    }
}

} // namespace renderer
} // namespace munchkin