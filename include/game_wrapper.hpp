#ifndef MUNCHKIN_GAME_WRAPPER_HPP__
#define MUNCHKIN_GAME_WRAPPER_HPP__

#include "api/gamerules.hpp"
#include "game.hpp"
#include "input/input.hpp"
#include "render_wrapper.hpp"
#include "systems/ai_manager.hpp"

#include <iostream>
#include <numeric>
#include <stdexcept>

#include "game_systems_cg.hpp"

struct SDL_Window;

namespace munchkin {

class GameWrapper {
public:
    // Defined in codegen'd systems_cg.hpp
    GameWrapper(size_t window_w,
                size_t window_h,
                size_t players_count,
                size_t ai_count,
                std::string gamerules_path = DEFAULT_GAMERULES_PATH);

    void main_loop(SDL_Window*);
    AIManager create_ai_manager(size_t players_count, size_t ai_count);

    bool show_debugger = false;

    Game game;
    AIManager ai_manager;
    RenderWrapper renderer;

    // Systems (codegen'd)
    MUNCHKIN_GAME_WRAPPER_SYSTEMS

    bool do_tick = false;
    bool done = false;

private:
    void load_all_systems_content();
};

} // namespace munchkin

#endif