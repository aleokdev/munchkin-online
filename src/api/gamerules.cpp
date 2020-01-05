#include <filesystem>

#include "api/gamerules.hpp"
#include "api/state.hpp"
#include <stdexcept>

namespace munchkin {

	GameRules::GameRules(State& state, std::string path)
	{
		std::filesystem::path fspath(path);
		fspath /= GAMERULES_GAMEFLOW_FILENAME;
		sol::protected_function_result result = state.lua.script_file(fspath.string());
		game_flow = result;
		state.active_coroutines.emplace_back(game_flow);
	}
}