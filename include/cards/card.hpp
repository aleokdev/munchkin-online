#pragma once
#include <string_view>
#include <map>
#include <memory>
#include <utility>
#include <sol/sol.hpp>
#include "../game/game.hpp"

namespace MnO::Cards
{
	struct Card
	{
		Card(Game::Game game, sol::load_result&& script) : _game(_game)
		{
			_script = std::move(script);
		}

		void OnPlay();
		void OnDiscard();
		void OnReveal();

	private:
		Game::Game& _game;
		std::unique_ptr<sol::load_result> _script;
	};
}