#ifndef MUNCHKIN_STATE_DEBUGGER_HPP__
#define MUNCHKIN_STATE_DEBUGGER_HPP__
#include "api/player.hpp"

namespace munchkin {

class GameWrapper;

namespace systems {

class [[system_codegen::wrapper_instance("state_debugger")]] StateDebugger {
public:
    StateDebugger(GameWrapper&);

    void load_content() {}

    void render();

private:
    GameWrapper* wrapper;
    bool show_demo = false;
    bool show_stateviewer = false;
    int current_selected_event = 0;
    PlayerPtr player_cards_to_show = nullptr;
};

} // namespace systems
} // namespace munchkin

#endif