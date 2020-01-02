#include <iostream>
#define SDL_MAIN_HANDLED
#include <sdl/SDL.h>

#include "api/munchkin.hpp"

int main()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cout << "Error initializing SDL: " << SDL_GetError() << std::endl;
		std::cin.get();
		return -1;
	}

	SDL_Window* window = SDL_CreateWindow("Munchkin Online", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 900, 600, NULL);

	SDL_Event event;
	bool close = false;
	do
	{
		SDL_PollEvent(&event);
		close = event.type == SDL_QUIT;
	} while (!close);

	return 0;
}
