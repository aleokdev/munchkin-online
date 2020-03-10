#ifndef MUNCHKIN_STATE_DEBUGGER_HPP__
#define MUNCHKIN_STATE_DEBUGGER_HPP__
#include "api/player.hpp"

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
    PlayerPtr player_cards_to_show = nullptr;
};

} // namespace systems
} // namespace munchkin

#endif