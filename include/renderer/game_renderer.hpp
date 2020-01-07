#ifndef MUNCHKIN_GAME_RENDERER_HPP__
#define MUNCHKIN_GAME_RENDERER_HPP__

namespace munchkin {

	struct Game;

	struct GameRenderer {
		GameRenderer(Game&);



	private:
		Game* game;
	};

}


#endif