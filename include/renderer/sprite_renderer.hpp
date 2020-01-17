#ifndef MUNCHKIN_RENDERER_SPRITE_RENDERER_HPP_
#define MUNCHKIN_RENDERER_SPRITE_RENDERER_HPP_

#include <glm/glm.hpp>

namespace munchkin {

namespace renderer {

/*
Example usage of this class:
renderer::SpriteRenderer r([&]() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, my_texture);
    r.do_draw();

    glBindTexture(GL_TEXTURE_2D, my_other_texture);
    r.do_draw();
});
*/
class SpriteRenderer {
public:
    SpriteRenderer();

    // All following functions require a bound shader

    void set_camera_drag(bool drag);
    // position in pixels from window origin (lower left corner is (0, 0)).
    void set_position(glm::vec2 pos);
    void set_scale(glm::vec2 multiplier);
    void set_rotation(float radians);
    void set_texture(unsigned int texture);
    void set_color(float r, float g, float b, float a);

    // Issue a single drawcall with the currently bound state.
    // Meant to be called from the render function passed in the constructor
    void do_draw();

    static void deallocate();

private:
    // shared data for vertices
    static unsigned int vao, vbo;

    static void init();

    static void setup_for_render();

    // Only x and y fields are actually used
    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::vec3 scale = glm::vec3(1, 1, 1);
    glm::vec4 color = glm::vec4(1, 1, 1, 1);

    // Only Z rotation really needed
    float rotation;
};

}

}

#endif