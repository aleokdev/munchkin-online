#include "renderer/sprite_renderer.hpp"

#include <glad/glad.h>

namespace munchkin::renderer {

unsigned int SpriteRenderer::vao = 0;
unsigned int SpriteRenderer::vbo = 0;

void SpriteRenderer::set_camera_drag(bool drag) {
    glUniform1i(4, (int)drag);
}

void SpriteRenderer::set_texture(unsigned int texture) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(3, 0);
}

void SpriteRenderer::set_position(float x, float y) {
    float v[] = {x, y};
    glUniform2fv(0, 1, v);
}

void SpriteRenderer::do_draw() {

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void SpriteRenderer::deallocate() {
    if (vao) {
        glDeleteVertexArrays(1, &vao);
    }
    if (vbo) {
        glDeleteVertexArrays(1, &vbo);
    }
}

void SpriteRenderer::init() {
    static float vertices[] = {
        // positions    texcoords
        -1, -1,         0, 0,
        -1, 1,          0, 1,
        1, -1,          1, 0,
        -1, 1,          0, 1,
        1, -1,          1, 0,
        1, 1,           1, 1                
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    // Fill buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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
}

void SpriteRenderer::setup_for_render() {
    if (!vao || !vbo) {
        init();
    }

    glBindVertexArray(vao);
}

}