#ifndef MUNCHKIN_RENDERER_BACKGROUND_RENDERER_HPP_
#define MUNCHKIN_RENDERER_BACKGROUND_RENDERER_HPP_

#include "assets/assets.hpp"

namespace munchkin {
namespace renderer {

class Background {
public:
    Background(assets::Handle<Texture> texture, bool scrolling = false);
    ~Background();

    assets::Handle<Texture> texture;

    void update_scroll(float delta_time);
    void render() const;

private:
    unsigned int vao;
    unsigned int vbo;
    unsigned int texcoords_buffer;
    unsigned int shader;

    bool scrolling;
    float scroll_speed = .02f;
    float scroll = 0;

    void create_buffers();
};

} // namespace renderer
} // namespace munchkin

#endif