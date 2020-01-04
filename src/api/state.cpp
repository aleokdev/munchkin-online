#include "api/state.hpp"

#include "api/card_loader.hpp"

#include <stdexcept>
#include <iostream>
#include <filesystem>

namespace munchkin  {

State::State(size_t player_count, std::string gamerule_path) : player_count(player_count) {
    // create players
    players.resize(player_count);
    for (int i = 0; i < player_count; ++i) {
        players[i].id = i;
    }
    current_player_id = 0;

    // register types in lua api
    lua.new_usertype<FlowEvent>("flow_event",
        "name", &FlowEvent::name);
    lua.new_usertype<State>("munchkin_state",
        "get_ticks", &State::get_ticks,
        "get_player", &State::get_player,
        "get_current_player", &State::get_current_player,
        "set_current_player", &State::set_current_player,
        "get_player_count", &State::get_player_count,
        "last_event", &State::last_event,
        "turn_number", &State::turn_number,
        "stage", &State::game_stage
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
    carddefs = load_cards(path, lua);
}

int State::get_ticks() const
{
	return tick;
}

void State::give_treasure(Player& player)
{
    // TODO
    throw std::runtime_error("give_treasure: Not implemented");
}

void State::give_dungeon(Player& player)
{
    // TODO
    throw std::runtime_error("give_dungeon: Not implemented");
}

void State::open_dungeon()
{
    // TODO
    throw std::runtime_error("open_dungeon: Not implemented");
}

Player& State::get_player(size_t id) {
    return players.at(id);
}

Player& State::get_current_player() {
    return players[current_player_id];
}

void State::set_current_player(size_t id)
{
    current_player_id = id;
}

size_t State::get_player_count() const {
    return player_count;
}

}