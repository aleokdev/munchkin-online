#include "renderer/sprite_renderer.hpp"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace munchkin::renderer {

unsigned int SpriteRenderer::vao = 0;
unsigned int SpriteRenderer::vbo = 0;

SpriteRenderer::SpriteRenderer() {
    setup_for_render();
}

void SpriteRenderer::set_camera_drag(bool drag) {
    glUniform1i(1, (int)drag);
}

void SpriteRenderer::set_position(glm::vec2 pos) {
    position = glm::vec3(pos, 0);
}

void SpriteRenderer::set_scale(glm::vec2 multiplier) {
    scale = glm::vec3(multiplier, 0);
}

void SpriteRenderer::set_rotation(float radians) {
    rotation = radians;
}

void SpriteRenderer::set_texture(unsigned int texture) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(0, 0);
}

void SpriteRenderer::set_color(float r, float g, float b, float a)
{
    color = glm::vec4(r, g, b, a);
    glUniform4fv(4, 1, glm::value_ptr(color));
}

void SpriteRenderer::do_draw() {
    // Calculate model matrix and send to shader
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, rotation, glm::vec3(0, 0, 1));
    model = glm::scale(model, scale);

    glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(model));

    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Reset color data (just in case)
    set_color(1, 1, 1, 1);
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