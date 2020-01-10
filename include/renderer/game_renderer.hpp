#ifndef MUNCHKIN_GAME_RENDERER_HPP__
#define MUNCHKIN_GAME_RENDERER_HPP__

#include "render_target.hpp"
#include "background_renderer.hpp"
#include "uniform_buffer.hpp"

namespace munchkin {

class Game;

class GameRenderer {
public:
	GameRenderer(Game&, size_t window_w, size_t window_h);
	~GameRenderer();

	void render_frame();
	void blit(unsigned int target_framebuf);

private:
	// windowing data
	size_t window_w, window_h;

	// Camera data
	struct Camera {
		// offsets are stored relative to center
		float xoffset;
		float yoffset;
	} camera;

	// Game-related data
	Game* game;

	// Input
	struct Mouse {
		int x, y;
		int pressed_button;
	};

	Mouse last_mouse;
	Mouse cur_mouse;

	// Timing
	float delta_time;
	float last_frame_time = 0;

	// Render data
	renderer::RenderTarget framebuf;
	renderer::UniformBuffer camera_buffer;

	// Assets
	renderer::Background background;
	unsigned int sprite_shader;

	// Functions
	void update_camera();
	void update_input();
};

}


#endif