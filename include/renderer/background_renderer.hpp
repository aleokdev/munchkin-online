#ifndef MUNCHKIN_RENDERER_BACKGROUND_RENDERER_HPP_
#define MUNCHKIN_RENDERER_BACKGROUND_RENDERER_HPP_

namespace munchkin::renderer {

struct Background {
    unsigned int texture;
    unsigned int vao;
    unsigned int vbo;
    unsigned int texcoords_buffer;
    unsigned int shader;
};

Background create_background(const char* bg_image_path);

void render_background(Background const& bg);

void free_background(Background& bg);

}


#endif