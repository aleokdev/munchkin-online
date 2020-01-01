#pragma once
#include "../player/class.hpp"

namespace MnO::Game
{
	struct GameRules
	{
		const int StartingLevel;
		const int MaxLevel;
		const PlayerClass DefaultClass;
	};
}