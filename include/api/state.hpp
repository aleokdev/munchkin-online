#ifndef MUNCHKIN_STATE_HPP_
#define MUNCHKIN_STATE_HPP_

#include <sol/sol.hpp>
#include <string_view>
#include "gamerules.hpp"

#include "player.hpp"
#include "carddef.hpp"

#define STATE_API_WRAPPER_FILE_PATH "data/generic/api_wrapper.lua"
#define STATE_API_RULES_FILE_NAME "rules.lua"

namespace munchkin {
struct FlowEvent {
    std::string name;
};

class State {
public:
    State(size_t player_count, std::string gamerule_path = DEFAULT_GAMERULES_PATH);

    void load_cards_from_json(std::string_view path);

    // api functions

    Player& get_player(size_t id);
    Player& get_current_player();
    void set_current_player(size_t id);

    size_t get_player_count() const;

    // data

    sol::state lua;
    sol::table game_api;
    FlowEvent last_event;
    size_t current_player_id;
    size_t player_count;
    size_t turn_number = 1;
    std::vector<Player> players;
    std::vector<CardDef> cards;
};

}

#endif