#ifndef MUNCHKIN_CARD_HPP_
#define MUNCHKIN_CARD_HPP_

#include <sol/sol.hpp>
#include <string>
#include "api/carddef.hpp"

namespace munchkin {

class State;
class Player;

// Cards refer to CardDefs
class Card {
public:
    Card(CardDef& def);
    Card(Card const&) = default;
    Card(Card&&) = default;

    Card& operator=(Card const&) = default;
    Card& operator=(Card&&) = default;

    ~Card() = default;

    CardDef& get_def() { return *def; }
    CardDef const& get_def() const { return *def; }

private:
    CardDef* def;
};

}

#endif