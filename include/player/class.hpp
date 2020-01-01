#pragma once
#include <string_view>
#include <memory>
#include <utility>
#include <sol/sol.hpp>
#include "../game/game.hpp"

namespace MnO::Player
{
	struct PlayerClass
	{
		PlayerClass(Game::Game& game, std::string_view name, std::size_t maxCards, sol::load_result&& _script) : _game(game), _name(name), _maxCards(maxCards)
		{
			script = std::move(_script);
		}

		const std::size_t get_MaxCards() { return _maxCards; }

	private:
		Game::Game& _game;
		const std::string_view _name;
		const std::size_t _maxCards;
		std::unique_ptr<sol::load_result> script;
	};
}