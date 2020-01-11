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
	char event_name[64] = "\0";
};

}

#endif