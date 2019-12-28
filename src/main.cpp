#include <iostream>
#include <SDL.h>

int main()
{
	std::cout << "insert game here" << std::endl;
	if (!SDL_Init(SDL_INIT_EVERYTHING))
	{
		std::cout << "Error initializing SDL: " << SDL_GetError() << std::endl;
		std::cin.get();
	}
	SDL_Window* window = SDL_CreateWindow("Munchkin Online", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 900, 600, NULL);

	SDL_Event event;
	bool close;
	do
	{
		SDL_PollEvent(&event);
		close = event.type == SDL_QUIT;
	} while (!close);

	return 0;
}
