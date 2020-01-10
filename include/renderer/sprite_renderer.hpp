#ifndef MUNCHKIN_RENDERER_SPRITE_RENDERER_HPP_
#define MUNCHKIN_RENDERER_SPRITE_RENDERER_HPP_

namespace munchkin::renderer {

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
   void set_position(float x, float y);
   void set_texture(unsigned int texture);

    // Issue a single drawcall with the currently bound state.
    // Meant to be called from the render function passed in the constructor
   void do_draw();

    static void deallocate();

private:
    // shared data for vertices
    static unsigned int vao, vbo;

    static void init();

    static void setup_for_render();
};

}

#endif