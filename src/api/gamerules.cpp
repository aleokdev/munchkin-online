#include <filesystem>

#include "api/gamerules.hpp"

namespace munchkin {

	GameRules::GameRules(sol::state& state, std::string path)
	{
		std::filesystem::path fspath(path);
		fspath /= GAMERULES_GAMEFLOW_FILENAME;
		game_flow_script = state.load_file(fspath.string());
		game_flow = game_flow_script;
	}

	void GameRules::continue_flow()
	{
		game_flow();
	}
}