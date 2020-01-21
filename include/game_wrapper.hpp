#ifndef MUNCHKIN_GAME_WRAPPER_HPP__
#define MUNCHKIN_GAME_WRAPPER_HPP__

#include "systems/game_renderer.hpp"
#include "systems/input_binder.hpp"
#include "systems/state_debugger.hpp"
#include "input/input.hpp"
#include "renderer/sprite_renderer.hpp"
#include "renderer/font_renderer.hpp"
#include "api/ai_manager.hpp"
#include "api/gamerules.hpp"
#include "game.hpp"

#include <stdexcept>
#include <numeric>
#include <iostream>

struct SDL_Window;

namespace munchkin {

class GameWrapper {
public:
	GameWrapper(size_t window_w, size_t window_h, size_t players_count, size_t ai_count, std::string gamerules_path = DEFAULT_GAMERULES_PATH);

	void main_loop(SDL_Window*);
	games::AIManager create_ai_manager(size_t players_count, size_t ai_count);

	bool show_debugger = false;

	Game game;

	// Systems

	systems::GameRenderer renderer;
	systems::InputBinder input_binder;
	systems::StateDebugger state_debugger;

	games::AIManager ai_manager;
};

}





#endif