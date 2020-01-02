#ifndef MUNCHKIN_GAME_HPP_
#define MUNCHKIN_GAME_HPP_

#include "state.hpp"

namespace munchkin {

class Game {
public:
    Game(size_t player_count);

    void turn();

    bool ended() const;
private:
    State state;
};

}

#endif