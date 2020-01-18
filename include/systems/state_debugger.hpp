#ifndef MUNCHKIN_STATE_DEBUGGER_HPP__
#define MUNCHKIN_STATE_DEBUGGER_HPP__

namespace munchkin {

	class Game;

namespace systems {

class StateDebugger {
public:
	StateDebugger(Game&);

	void render();

private:
	Game* game;
	bool show_demo = false;
	bool show_stateviewer = false;
	int current_selected_event = 0;
};

}
}

#endif