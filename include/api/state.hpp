#ifndef MUNCHKIN_STATE_HPP_
#define MUNCHKIN_STATE_HPP_

#include <sol/sol.hpp>

#include "player.hpp"

namespace munchkin {

class State {
public:
    State(size_t player_count);

    // api functions

    Player& get_player(size_t id);

    // data

    sol::state lua;
    bool game_ended = false;
    std::vector<Player> players;
};

}

#endif