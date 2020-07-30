#ifndef MUNCHKIN_RENDERER_BACKGROUND_RENDERER_HPP_
#define MUNCHKIN_RENDERER_BACKGROUND_RENDERER_HPP_

#include <glm/vec4.hpp>

#include "assets/assets.hpp"

namespace munchkin {
namespace renderer {

class Background {
public:
    Background();
    Background(assets::Handle<Texture> texture, bool scrolling = false);
    ~Background();

    static void load_content();

    assets::Handle<Texture> texture;
    glm::vec4 color = {1, 1, 1, 1};

    void update_scroll(float delta_time);
    void render() const;

private:
    unsigned int vao;
    unsigned int vbo;
    unsigned int texcoords_buffer;
    static inline assets::Handle<renderer::Shader> shader;

    bool scrolling;
    float scroll_speed = .02f;
    float scroll = 0;

    void create_buffers();
};

} // namespace renderer
} // namespace munchkin

#endif