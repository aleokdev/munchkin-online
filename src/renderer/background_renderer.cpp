#include "renderer/background_renderer.hpp"

#include <stb/stb_image.h>
#include <glad/glad.h>

#include <string>
#include <fstream>

#include <iostream>

namespace munchkin::renderer {

static unsigned int load_background_texture(const char* path) {
    stbi_set_flip_vertically_on_load(true);
    int w, h, channels;
    unsigned char* data = stbi_load(path, &w, &h, &channels, 4);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    return texture;
}

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

// TODO: Move shader loading code to different file

static std::string read_file(const char* path) {
    using namespace std::literals::string_literals;
    std::ifstream f(path);
    if (!f.good()) {
        throw std::runtime_error("Failed to open file: "s + path);
    }
    return std::string(std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>());
}

unsigned int create_shader_stage(GLenum stage, const char* source) {
    using namespace std::literals::string_literals;
    unsigned int shader = glCreateShader(stage);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    char infolog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infolog);
        throw std::runtime_error("Failed to compile shader:\n"s + source + "\nReason: "s + infolog);
    }

    return shader;
}

unsigned int create_shader(const char* vertex, const char* fragment) {
    using namespace std::literals::string_literals;
    unsigned int vtx = create_shader_stage(GL_VERTEX_SHADER, vertex);
    unsigned int frag = create_shader_stage(GL_FRAGMENT_SHADER, fragment);

    unsigned int prog = glCreateProgram();
    glAttachShader(prog, vtx);
    glAttachShader(prog, frag);

    glLinkProgram(prog);
    int success;
    char infolog[512];
    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(prog, 512, nullptr, infolog);
        throw std::runtime_error("Failed to link shader.\nReason: "s + infolog);
        return 0;
    }

    glDeleteShader(vtx);
    glDeleteShader(frag);

    return prog;
}


Background create_background(const char* bg_image_path) {
    Background bg;

    bg.texture = load_background_texture(bg_image_path);
    create_background_buffers(bg);
    std::string vert = read_file("data/shaders/background.vert");
    std::string frag = read_file("data/shaders/background.frag");
    bg.shader = create_shader(vert.c_str(), frag.c_str());

    return bg;
}

void render_background(Background const& bg) {
    glUseProgram(bg.shader);
    glBindVertexArray(bg.vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bg.texture);
    glUniform1i(0, 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void free_background(Background& bg) {
    glDeleteTextures(1, &bg.texture);
    glDeleteVertexArrays(1, &bg.vao);
    glDeleteBuffers(1, &bg.vbo);
    glDeleteBuffers(1, &bg.texcoords_buffer);
    glDeleteProgram(bg.shader);
}

}