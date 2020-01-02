#include "api/state.hpp"

namespace munchkin  {

State::State(size_t player_count) : player_count(player_count) {
    // create players
    players.resize(player_count);
    for (int i = 0; i < player_count; ++i) {
        players[i].id = i;
    }

    // register types in lua api
    lua.new_usertype<State>("munchkin_state", 
        "game_ended", &State::game_ended, 
        "get_player", &State::get_player,
        "get_current_player", &State::get_current_player,
        "get_player_count", &State::get_player_count
        );
    lua.new_usertype<Player>("munchkin_player", "level", &Player::level, "id", &Player::id);

    sol::table game_api = lua.script_file("data/scripts/game.lua");
}


Player& State::get_player(size_t id) {
    return players[id];
}

Player& State::get_current_player() {
    return players[current_player_id];
}

size_t State::get_player_count() const {
    return player_count;
}

void State::load_json(std::string_view path)
{
    // TODO: this
}

void State::load_card_script(std::string_view path, std::string name, std::string description)
{
    cards.emplace_back(lua, path, name, description);
}

}