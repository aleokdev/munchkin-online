#ifndef MUNCHKIN_GAME_HPP_
#define MUNCHKIN_GAME_HPP_

#include "state.hpp"

namespace munchkin {

class Game {
public:
    Game(size_t player_count);

    void turn();

    bool ended();

    State const& get_state() const { return state; }

private:
    State state;
};

}

#endif