#ifndef MUNCHKIN_FONT_RENDERER_HPP__
#define MUNCHKIN_FONT_RENDERER_HPP__

#include <string>
#include <glm/glm.hpp>
#include "renderer/font.hpp"

namespace munchkin {
namespace renderer {

class FontRenderer {
public:
	FontRenderer();

	// All following functions require a bound shader

	void set_camera_drag(bool drag) {}
	// position in pixels from window origin (lower left corner is (0, 0)).
	void set_position(glm::vec2 pos) {}
	void set_scale(glm::vec2 multiplier) {}
	void set_rotation(float radians) {}

	// Issue many drawcalls with the currently bound state.
	// Meant to be called from the render function passed in the constructor
	// Call after calling set_x functions.
	void render_text(Font& fnt, std::string txt) {}

	// Issue a single drawcall with the currently bound state.
	// Meant to be called from the render function passed in the constructor
	// Call after calling set_x functions.
	void render_glyph(Font::glyph_data const& glyph, glm::vec2 offset) {}

	static void deallocate();

private:
	// shared data for vertices
	static inline unsigned int vao, vbo = 0;

	static unsigned int vert_shader, frag_shader;

	static void init();

	static void setup_for_render();

	glm::vec2 position = glm::vec2(0, 0);
	glm::vec2 scale = glm::vec2(1, 1);

	// Only Z rotation really needed
	float rotation;
};

}
}

#endif