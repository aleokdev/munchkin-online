#pragma once
#include <stdio.h>
#include <memory>
#include "class.hpp"

namespace MnO::Player
{
	struct Player
	{
		void get_CardMax() { return _cardMax; }

	private:
		int _level;
		std::size_t _cardMax;
		std::unique_ptr<PlayerClass> _class;
	};
}