#ifndef MUNCHKIN_GAME_HPP_
#define MUNCHKIN_GAME_HPP_

#include "state.hpp"
#include "gamerules.hpp"

namespace munchkin {

class Game {
public:
    Game(size_t player_count, std::string gamerules_path = DEFAULT_GAMERULES_PATH);

    void turn();

    void tick();

    void push_event(FlowEvent e);

    bool ended();

    State const& get_state() const { return state; }

private:
    State state;
    GameRules gamerules;
};

}

#endif