#include "api/state.hpp"

#include "api/card_loader.hpp"

#include <stdexcept>

namespace munchkin  {

State::State(size_t player_count) : player_count(player_count) {
    // create players
    players.resize(player_count);
    for (int i = 1; i < player_count; ++i) {
        players[i].id = i;
    }
    current_player = players.begin();

    // register types in lua api
    lua.new_usertype<State>("munchkin_state", 
        "get_player", &State::get_player,
        "get_current_player", &State::get_current_player,
        "get_player_count", &State::get_player_count
        );
    lua.new_usertype<Player>("munchkin_player", "level", &Player::level, "id", &Player::id);
    
    // Load the gamerule's API
    lua["state"] = this;
    game_api = lua.script_file("data/gamerules/default/game.lua");
    if (!game_api.is<sol::table>())
        throw std::runtime_error("Type returned from game.lua is NOT a table!");
}

void State::load_cards_from_json(std::string_view path) {
    cards = load_cards(path, lua);
}

Player& State::get_player(size_t id) {
    return players.at(id);
}

Player& State::get_current_player() {
    return *current_player;
}

size_t State::get_player_count() const {
    return player_count;
}

void State::next_player_turn()
{
    if (current_player == players.end())
        current_player = players.begin();
    else
        current_player++;
}

}