#ifndef MUNCHKIN_STATE_DEBUGGER_HPP__
#define MUNCHKIN_STATE_DEBUGGER_HPP__

namespace munchkin {

struct State;

struct StateDebugger {
	StateDebugger(State&);

	void render();

private:
	State* state;
	bool show_demo = false;
	char event_name[64] = "\0";
};

}

#endif