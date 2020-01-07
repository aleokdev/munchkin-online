#ifndef MUNCHKIN_GAME_RENDERER_HPP__
#define MUNCHKIN_GAME_RENDERER_HPP__

namespace munchkin {

struct Game;

class GameRenderer {
public:
	GameRenderer(Game&);

	void render_frame();

private:
	Game* game;
};

}


#endif