#ifndef MUNCHKIN_GAMERULES_HPP__
#define MUNCHKIN_GAMERULES_HPP__

#include <string>
#include <sol/sol.hpp>

// Defines the default gamerules used.
#define DEFAULT_GAMERULES_PATH "data/gamerules/default/"
#define GAMERULES_GAMEFLOW_FILENAME "game_flow.lua"

namespace munchkin {
	struct Game;

	struct GameRules {
		GameRules(sol::state& state, std::string path);

		void continue_flow(Game& game);

	private:
		sol::load_result game_flow_script;
	};
}

#endif