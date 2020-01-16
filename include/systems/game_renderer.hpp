#ifndef MUNCHKIN_GAME_RENDERER_HPP__
#define MUNCHKIN_GAME_RENDERER_HPP__

#include "renderer/render_target.hpp"
#include "renderer/background_renderer.hpp"
#include "renderer/uniform_buffer.hpp"
#include "renderer/sprite_renderer.hpp"
#include "input/input.hpp"

#include <glm/mat4x4.hpp>

namespace munchkin {

class Game;

namespace systems {

class GameRenderer {
public:
	GameRenderer(Game&);
	~GameRenderer();

	void render_frame();
	void blit(unsigned int target_framebuf);

	void on_resize(size_t w, size_t h);

private:
	glm::mat4 projection;

	// Game-related data
	Game* game;

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

	// Functions
	void update_camera();
	void update_input();
	void draw_cards(renderer::SpriteRenderer&);
};

}
}


#endif