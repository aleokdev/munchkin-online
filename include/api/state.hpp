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

    // TODO: Try to stay away from IDs! They cause confusion! (Should they start at 0 or 1?)
    Player& get_player(size_t id);
    Player& get_current_player();
    void next_player_turn();

    size_t get_player_count() const;

    // data

    sol::state lua;
    sol::table game_api;
    std::vector<Player>::iterator current_player;
    size_t player_count;
    std::vector<Player> players;
    std::vector<CardDef> cards;
};

}

#endif