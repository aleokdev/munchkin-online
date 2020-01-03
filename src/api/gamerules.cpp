#include <filesystem>

#include "api/gamerules.hpp"
#include <stdexcept>

namespace munchkin {

	GameRules::GameRules(sol::state& state, std::string path)
	{
		std::filesystem::path fspath(path);
		fspath /= GAMERULES_GAMEFLOW_FILENAME;
		sol::protected_function_result result = state.script_file(fspath.string());
		game_flow = result;
	}

	void GameRules::continue_flow()
	{
		game_flow();
	}
}