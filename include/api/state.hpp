#ifndef MUNCHKIN_STATE_HPP_
#define MUNCHKIN_STATE_HPP_

#include <sol/sol.hpp>
#include <string_view>
#include <queue>
#include <optional>
#include "gamerules.hpp"

#include "player.hpp"
#include "carddef.hpp"
#include "card.hpp"
#include "battle.hpp"

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
    // More information in scripting_api.md

    int get_ticks() const;

    void give_treasure(Player& player);
    void give_dungeon(Player& player);
    void open_dungeon();

    void start_battle();
    void end_current_battle();

    Player& get_player(size_t id);
    size_t get_player_count() const;
    Player& get_current_player();
    void set_current_player(size_t id);
    // next_player_turn() defined in api_wrapper.lua

    std::vector<CardPtr> get_visible_cards();

    // data

    sol::state lua;
    sol::table game_api;
    bool should_borrow_facing_up;
    std::string game_stage;
    size_t turn_number = 1;
    size_t tick = 0;

    size_t generate_id() {
        return last_id++;
    }
    size_t last_id = 0;

    std::vector<Player> players;
    size_t current_player_id;
    size_t player_count;

    std::vector<CardDef> carddefs;

    void add_cardpack(std::string path);
    Card& add_card(CardDef& def);
    std::vector<Card> all_cards;

    std::queue<CardPtr> dungeon_deck;
    std::queue<CardPtr> dungeon_discard_deck;
    std::queue<CardPtr> treasure_deck;
    std::queue<CardPtr> treasure_discard_deck;

    std::queue<FlowEvent> event_queue;
    FlowEvent last_event;
    std::vector<sol::coroutine> active_coroutines;

    size_t default_hand_max_cards;

    // current_battle is not a pointer because i couldn't get std::unique_ptr to work with sol :(
    std::optional<Battle> current_battle;
};

}

#endif