#pragma once
#include <stdio.h>
#include <memory>
#include "class.hpp"

namespace MnO
{
	namespace Player
	{
		struct Player
		{
			virtual ~Player() {}

			const std::size_t get_CardMax() { return _cardMax; }

		protected:
			int _level;
			std::size_t _cardMax;
			std::unique_ptr<PlayerClass> _class;
		};
	}
}