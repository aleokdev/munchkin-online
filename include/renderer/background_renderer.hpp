#ifndef MUNCHKIN_RENDERER_BACKGROUND_RENDERER_HPP_
#define MUNCHKIN_RENDERER_BACKGROUND_RENDERER_HPP_

#include "assets.hpp"

namespace munchkin {
namespace renderer {

struct Background {
    assets::Handle<Texture> texture;
    unsigned int vao;
    unsigned int vbo;
    unsigned int texcoords_buffer;
    unsigned int shader;

    float scroll_speed = 0;

private:
    float scroll = 0;

    friend void update_background_scroll(Background& bg, float delta_time);
    friend void render_background(Background const& bg);
};

Background create_background(assets::Handle<Texture> texture);

void update_background_scroll(Background& bg, float delta_time);

void render_background(Background const& bg);

void free_background(Background& bg);

} // namespace renderer
} // namespace munchkin

#endif