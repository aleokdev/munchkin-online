#ifndef MUNCHKIN_STATE_HPP_
#define MUNCHKIN_STATE_HPP_

#include <sol/sol.hpp>
#include <string_view>

#include "player.hpp"
#include "carddef.hpp"

namespace munchkin {

class State {
public:
    State(size_t player_count);

    void load_cards_from_json(std::string_view path);

    // api functions

    Player& get_player(size_t id);
    Player& get_current_player();

    size_t get_player_count() const;

    // data

    sol::state lua;
    sol::table game_api;
    bool game_ended = false;
    size_t current_player_id = 0;
    size_t player_count;
    std::vector<Player> players;
    std::vector<CardDef> cards;
};

}

#endif