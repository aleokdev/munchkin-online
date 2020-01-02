#include "api/card.hpp"

#include "api/player.hpp"
#include "api/state.hpp"

namespace munchkin {

Card::Card(std::string internal_name, std::string const& script_path, sol::state& lua) 
    : internal_name(std::move(internal_name)) {
    lua.script_file(script_path);
}

void Card::play(State& state, Player& player) {
    state.lua[internal_name]["on_play"](state, player);
}

}