#ifndef MUNCHKIN_GAME_RENDERER_HPP__
#define MUNCHKIN_GAME_RENDERER_HPP__

#include "render_target.hpp"
#include "background_renderer.hpp"
#include "uniform_buffer.hpp"
#include "input/input.hpp"
#include "card_sprite.hpp"

#include <glm/mat4x4.hpp>

namespace munchkin {

class State;

class GameRenderer {
public:
	GameRenderer(State&, size_t window_w, size_t window_h);
	~GameRenderer();

	void render_frame();
	void blit(unsigned int target_framebuf);

	void on_resize(size_t w, size_t h);

private:
	// windowing data
	size_t window_w, window_h;

	// Camera data
	struct Camera {
		Camera(float x, float y) : xoffset(x), yoffset(y) {}
		// offsets are stored relative to center
		float xoffset;
		float yoffset;
	} camera;

	glm::mat4 projection;

	// Game-related data
	State* state;

	input::MouseState last_mouse;
	input::MouseState cur_mouse;

	// Timing
	float delta_time;
	float last_frame_time = 0;

	// Render data
	renderer::RenderTarget framebuf;
	renderer::UniformBuffer camera_buffer;
	unsigned int table_texture;

	// Assets
	renderer::Background background;
	unsigned int sprite_shader;

	// Sprites
	std::vector<renderer::CardSprite> card_sprites;

	// Functions
	void update_camera();
	void update_input();
	void draw_cards(renderer::SpriteRenderer&);
};

}


#endif