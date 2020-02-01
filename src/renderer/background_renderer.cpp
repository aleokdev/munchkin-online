#include "renderer/background_renderer.hpp"

#include "renderer/util.hpp"

#include <glad/glad.h>
#include <stb/stb_image.h>

#include <fstream>
#include <string>

namespace munchkin::renderer {

static void create_background_buffers(Background& bg) {
    // we will specify the vertices as vec2's
    static float vertices[] = {-1, -1, -1, 1, 1, -1, -1, 1, 1, -1, 1, 1};
    static float tex_coords[] = {0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1};
    unsigned int vbo, vao, texcoords_buf;

    glGenBuffers(1, &vbo);
    glGenBuffers(1, &texcoords_buf);
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
    glBindBuffer(GL_ARRAY_BUFFER, texcoords_buf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords), tex_coords, GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, 0);
    glBindVertexBuffer(1, texcoords_buf, 0, 2 * sizeof(float));
    glVertexAttribBinding(1, 1);

    bg.vbo = vbo;
    bg.vao = vao;
    bg.texcoords_buffer = texcoords_buf;
}

Background create_background(assets::Handle<Texture> texture) {
    Background bg;

    bg.texture = texture;
    create_background_buffers(bg);
    bg.shader = load_shader("data/shaders/background.vert", "data/shaders/background.frag");

    return bg;
}

void update_background_scroll(Background& bg, float delta_time) {
    bg.scroll += bg.scroll_speed * delta_time;

    // prevent eventual overflow. We only do this after scroll > 100 so we don't have to reset too
    // often
    if (bg.scroll > 100.0f) {
        bg.scroll -= 100.0f;
    }
}

void render_background(Background const& bg) {
    glUseProgram(bg.shader);
    glBindVertexArray(bg.vao);

    auto& texture = assets::get_manager<Texture>().get_asset(bg.texture);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.handle);
    glUniform1i(0, 0);

    glUniform1f(1, bg.scroll);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void free_background(Background& bg) {
    glDeleteVertexArrays(1, &bg.vao);
    glDeleteBuffers(1, &bg.vbo);
    glDeleteBuffers(1, &bg.texcoords_buffer);
    glDeleteProgram(bg.shader);
}

} // namespace munchkin::renderer