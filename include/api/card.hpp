#ifndef MUNCHKIN_CARD_HPP_
#define MUNCHKIN_CARD_HPP_

#include <sol/sol.hpp>
#include <string>

namespace munchkin {

class State;
class Player;

class Card {
public:
    Card(std::string const& script_path, sol::state& lua);

    void play(State& state, Player& player);
};

}

#endif