#ifndef MUNCHKIN_PLAYER_HPP_
#define MUNCHKIN_PLAYER_HPP_

namespace munchkin {

class State;
class Card;

class Player {
private:
public:
    int level = 1;
    size_t id = 0;
};

}

#endif