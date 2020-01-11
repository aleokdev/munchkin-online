#ifndef MUNCHKIN_STATE_DEBUGGER_HPP__
#define MUNCHKIN_STATE_DEBUGGER_HPP__

namespace munchkin {

class State;

class StateDebugger {
public:
	StateDebugger(State&);

	void render();

private:
	State* state;
	bool show_demo = false;
	bool show_stateviewer = false;
	int current_selected_event = 0;
};

}

#endif