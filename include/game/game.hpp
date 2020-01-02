#pragma once
#include <vector>
#include <memory>
#include "../player/player.hpp"

namespace MnO
{
	namespace Game
	{
		struct Game
		{
			virtual ~Game() {}
			const sol::state& get_lua() const { return _lua; }

		protected:
			sol::state _lua;
			
			std::vector<std::unique_ptr<Player::Player>> _players;
		};

		struct AIGame : public Game
		{
			AIGame(int players);
			~AIGame() {}
		};
	}
}