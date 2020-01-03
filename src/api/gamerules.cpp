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
		sol::protected_function_result result = game_flow();
		if (!result.valid())
		{
			std::cout << "There was an error in the game_flow script, and it has died." << std::endl;
		}
	}
}