#include "renderer/background_renderer.hpp"

#include "renderer/util.hpp"

#include <glad/glad.h>
#include <stb/stb_image.h>

#include <fstream>
#include <string>

namespace munchkin {
namespace renderer {

void Background::create_buffers() {
    // we will specify the vertices as vec2's
    static float vertices[] = {-1, -1, -1, 1, 1, -1, -1, 1, 1, -1, 1, 1};
    static float tex_coords[] = {0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1};

    glGenBuffers(1, &vbo);
    glGenBuffers(1, &texcoords_buffer);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

    // positions
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, 0);
    glBindVertexBuffer(0, vbo, 0, 2 * sizeof(float));
    glVertexAttribBinding(0, 0);
    // texcoords
    glBindBuffer(GL_ARRAY_BUFFER, texcoords_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords), tex_coords, GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, 0);
    glBindVertexBuffer(1, texcoords_buffer, 0, 2 * sizeof(float));
    glVertexAttribBinding(1, 1);
}

Background::Background(assets::Handle<Texture> tex) : texture(tex) {
    create_buffers();
    shader = load_shader("data/shaders/background.vert", "data/shaders/background.frag");
}

void Background::update_scroll(float delta_time) {
    scroll += scroll_speed * delta_time;

    // prevent eventual overflow. We only do this after scroll > 100 so we don't have to reset too
    // often
    if (scroll > 100.0f) {
        scroll -= 100.0f;
    }
}

void Background::render() const {
    glUseProgram(shader);
    glBindVertexArray(vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, assets::get_manager<Texture>().get_asset(texture).handle);
    glUniform1i(0, 0);

    glUniform1f(1, scroll);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

Background::~Background() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &texcoords_buffer);
    glDeleteProgram(shader);
}

} // namespace renderer
} // namespace munchkin