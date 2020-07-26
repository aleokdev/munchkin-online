#ifndef MUNCHKIN_GAME_WRAPPER_HPP__
#define MUNCHKIN_GAME_WRAPPER_HPP__

#include "api/gamerules.hpp"
#include "game.hpp"
#include "input/input.hpp"
#include "render_wrapper.hpp"
#include "renderer/font_renderer.hpp"
#include "renderer/sprite_renderer.hpp"
#include "systems/ai_manager.hpp"
#include "systems/debug_terminal.hpp"
#include "systems/game_logger.hpp"
#include "systems/input_binder.hpp"
#include "systems/state_debugger.hpp"

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

    // Systems

    RenderWrapper renderer;
    Game game;
    systems::InputBinder input_binder;
    systems::StateDebugger state_debugger;
    systems::DebugTerminal debug_terminal;
    systems::GameLogger logger;

    AIManager ai_manager;

    bool do_tick = false;
    bool done = false;
};

} // namespace munchkin

#endif