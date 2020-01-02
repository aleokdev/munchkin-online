#include <iostream>
#define SDL_MAIN_HANDLED
#include <sdl/SDL.h>

#include "api/munchkin.hpp"

int main()
{
	munchkin::Game game(4);

	std::cout << std::boolalpha;
	std::cout << "Game over: " << game.ended() << std::endl;

	while(!game.ended()) {
		game.turn();
	}

	std::cout << "Game over: " << game.ended() << std::endl;

	std::cout << "Levels: " << std::endl;
	for (int i = 0; i < 4; ++i) {
		std::cout << i << ": " << game.get_state().players[i].level << "\n";
	}

	return 0;
}
