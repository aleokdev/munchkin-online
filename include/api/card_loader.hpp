#ifndef MUNCHKIN_CARD_LOADER_HPP_
#define MUNCHKIN_CARD_LOADER_HPP_

#include "carddef.hpp"

#include <sol/sol.hpp>
#include <string_view>
#include <vector>

namespace munchkin {

std::vector<CardDef> load_cards(std::string_view path, sol::state& lua);

}

#endif