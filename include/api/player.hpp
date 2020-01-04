#ifndef MUNCHKIN_PLAYER_HPP_
#define MUNCHKIN_PLAYER_HPP_

namespace munchkin {

class State;
class Card;

struct Player {
    int level = 1;
    size_t id = 0;

    int get_power() { return level; }
};

}

#endif