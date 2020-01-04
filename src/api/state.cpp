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

    sol::usertype<State> state_type = lua.new_usertype<State>("munchkin_state",
        "last_event", &State::last_event,
        "get_ticks", &State::get_ticks,
        "stage", &State::game_stage,
        "turn_number", &State::turn_number,

        "give_treasure", &State::give_treasure,
        "give_dungeon", &State::give_dungeon,
        "open_dungeon", &State::open_dungeon,
        "should_borrow_facing_up", &State::should_borrow_facing_up,

        "start_battle", &State::start_battle,
        "current_battle", &State::current_battle,
        "end_current_battle", &State::end_current_battle,

        "get_player", &State::get_player,
        "get_current_player", &State::get_current_player,
        "set_current_player", &State::set_current_player,
        "get_player_count", &State::get_player_count
        );

    lua.new_usertype<Player>("munchkin_player",
        "level", &Player::level,
        "id", &Player::id);

    lua.new_usertype<Battle>("munchkin_battle",
        "player_power_offset", &Battle::player_power_offset,
        "monster_power_offset", &Battle::monster_power_offset,
        "get_total_player_power", &Battle::get_total_player_power,
        "get_total_monster_power", &Battle::get_total_monster_power);

    lua.open_libraries(sol::lib::coroutine);
    
    // Load the generic API wrapper
    // TODO: Rename to state?
    lua["game"] = this;
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

void State::start_battle()
{
    // TODO
    throw std::runtime_error("start_battle: Not implemented");
}

void State::end_current_battle()
{
    // TODO
    throw std::runtime_error("end_current_battle: Not implemented");
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