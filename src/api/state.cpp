#include "api/state.hpp"

#include "api/card_loader.hpp"

#include <stdexcept>
#include <iostream>
#include <filesystem>

namespace munchkin  {

State::State(size_t player_count, std::string gamerule_path) : player_count(player_count) {
    // create players
    players.resize(player_count);
    for (int i = 1; i < player_count; ++i) {
        players[i].id = i;
    }
    current_player = players.begin();

    // register types in lua api
    lua.new_usertype<FlowEvent>("flow_event",
        "name", &FlowEvent::name);
    lua.new_usertype<State>("munchkin_state", 
        "get_player", &State::get_player,
        "get_current_player", &State::get_current_player,
        "get_player_count", &State::get_player_count,
        "last_event", &State::last_event
        );
    lua.new_usertype<Player>("munchkin_player", "level", &Player::level, "id", &Player::id);
    lua.open_libraries(sol::lib::coroutine);
    
    // Load the generic API wrapper
    lua["state"] = this;
    lua.script_file(STATE_API_WRAPPER_FILE_PATH);

    // Load the gamerule's API
    std::filesystem::path fspath(gamerule_path);
    fspath /= STATE_API_RULES_FILE_NAME;
    lua.script_file(fspath.string());

    game_api = lua["game"];
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

}