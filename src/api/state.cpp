#include "api/state.hpp"

namespace munchkin  {

State::State(size_t player_count) {
    // create players
    players.resize(player_count);
    for (int i = 0; i < player_count; ++i) {
        players[i].id = i;
    }

    // register types in lua api
    lua.new_usertype<State>("munchkin_state", "game_ended", &State::game_ended, "get_player", &State::get_player);
    lua.new_usertype<Player>("munchkin_player", "level", &Player::level, "id", &Player::id);

    lua.script_file("data/scripts/card.lua");
}


Player& State::get_player(size_t id) {
    return players[id];
}

}