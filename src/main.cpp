#include <iostream>
#define SDL_MAIN_HANDLED

#include "api/munchkin.hpp"

int main()
{
	munchkin::Game game(4);
	game.get_state().add_cardpack("data/cardpacks/default/cards.json");

	std::cout << "Cards loaded: " << game.get_state().carddefs.size() << std::endl;

	while(!game.ended()) {
		game.turn();
	}

	std::cout << std::boolalpha;
	std::cout << "Game over: " << game.ended() << std::endl;

	std::cout << "Levels: " << std::endl;
	for (int i = 0; i < 4; ++i) {
		std::cout << i << ": " << game.get_state().players[i].level << "\n";
	}

	return 0;
}
