#pragma once
#include <string_view>
#include <memory>
#include <utility>
#include <sol/sol.hpp>


namespace MnO
{
	namespace Game
	{
		struct Game;
	}

	namespace Player
	{
		struct PlayerClass
		{
			PlayerClass(MnO::Game::Game& game, std::string_view name, std::size_t maxCards, sol::load_result _script) : _game(game), _name(name), _maxCards(maxCards), script(_script)
			{
			}

			const std::size_t get_MaxCards() { return _maxCards; }

		private:
			MnO::Game::Game& _game;
			const std::string_view _name;
			const std::size_t _maxCards;
			sol::load_result script;
		};
	}
}