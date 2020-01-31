#ifndef MUNCHKIN_GAME_WRAPPER_HPP__
#define MUNCHKIN_GAME_WRAPPER_HPP__

#include "api/gamerules.hpp"
#include "game.hpp"
#include "input/input.hpp"
#include "renderer/font_renderer.hpp"
#include "renderer/sprite_renderer.hpp"
#include "systems/game_renderer.hpp"
#include "systems/input_binder.hpp"
#include "systems/state_debugger.hpp"
#include "systems/debug_terminal.hpp"
#include "systems/ai_manager.hpp"

#include <iostream>
#include <numeric>
#include <stdexcept>

struct SDL_Window;

namespace munchkin {

class GameWrapper {
public:
    GameWrapper(size_t window_w,
                size_t window_h,
                size_t players_count,
                size_t ai_count,
                std::string gamerules_path = DEFAULT_GAMERULES_PATH);

    void main_loop(SDL_Window*);
    AIManager create_ai_manager(size_t players_count, size_t ai_count);

    bool show_debugger = false;

    Game game;

    // Systems

    systems::GameRenderer renderer;
    systems::InputBinder input_binder;
    systems::StateDebugger state_debugger;
    systems::DebugTerminal debug_terminal;

    AIManager ai_manager;

    bool done = false;
};

} // namespace munchkin

#endif