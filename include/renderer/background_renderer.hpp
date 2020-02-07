#ifndef MUNCHKIN_RENDERER_BACKGROUND_RENDERER_HPP_
#define MUNCHKIN_RENDERER_BACKGROUND_RENDERER_HPP_

#include "assets.hpp"

namespace munchkin {
namespace renderer {

class Background {
public:
    Background(assets::Handle<Texture> texture);
    ~Background();

    assets::Handle<Texture> texture;

    void update_scroll(float delta_time);
    void render() const;

    inline static const assets::loaders::LoadParams<Texture> default_load_params = {
        "data/generic/bg.png"};

private:
    unsigned int vao;
    unsigned int vbo;
    unsigned int texcoords_buffer;
    unsigned int shader;

    float scroll_speed = 0;
    float scroll = 0;

    void create_buffers();
};

} // namespace renderer
} // namespace munchkin

#endif