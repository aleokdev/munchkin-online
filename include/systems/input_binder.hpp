#ifndef MUNCHKIN_INPUT_BINDER_HPP__
#define MUNCHKIN_INPUT_BINDER_HPP__

namespace munchkin {

class Game;

namespace systems {

// Binds input from the user to Game and its state
class InputBinder {
	InputBinder(Game&);

	void tick();

private:
	Game* const game;
};

}
}

#endif