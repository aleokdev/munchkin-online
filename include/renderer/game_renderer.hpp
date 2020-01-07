#ifndef MUNCHKIN_GAME_RENDERER_HPP__
#define MUNCHKIN_GAME_RENDERER_HPP__

#include "render_target.hpp"
#include "background_renderer.hpp"

namespace munchkin {

struct Game;

class GameRenderer {
public:
	GameRenderer(Game&, size_t window_w, size_t window_h);
	~GameRenderer();

	void render_frame();
	void blit(unsigned int target_framebuf);

private:
	Game* game;
	renderer::RenderTarget framebuf;

	renderer::Background background;
};

}


#endif