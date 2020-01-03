#include "api/card.hpp"

#include "api/player.hpp"
#include "api/state.hpp"

namespace munchkin {

Card::Card(CardDef& def) : def(&def), data(sol::metatable(def.metatable)) {}


}