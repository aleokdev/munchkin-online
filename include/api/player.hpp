#ifndef MUNCHKIN_PLAYER_HPP_
#define MUNCHKIN_PLAYER_HPP_

namespace munchkin {

class State;
class CardPtr;

struct Player {
    int level = 1;
    size_t id = 0;

    int get_power() { return level; }

    std::vector<CardPtr> hand;
    std::vector<CardPtr> equipped;
    size_t hand_max_cards;
};

}

#endif