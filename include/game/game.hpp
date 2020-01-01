#pragma once
#include <vector>
#include "../player/player.hpp"

namespace MnO::Game
{
	struct Game
	{
		const sol::state& get_lua() { return _lua; }

	protected:
		sol::state _lua;
		std::vector<std::unique_ptr<Player::Player>> _players;
	};
}