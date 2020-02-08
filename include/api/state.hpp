#ifndef MUNCHKIN_STATE_HPP_
#define MUNCHKIN_STATE_HPP_

#include "gamerules.hpp"
#include <optional>
#include <queue>
#include <sol/sol.hpp>
#include <string_view>
#include <filesystem>

#include "battle.hpp"
#include "card.hpp"
#include "carddef.hpp"
#include "player.hpp"

#define STATE_API_WRAPPER_FILE_PATH "data/generic/api_wrapper.lua"
#define STATE_API_RULES_FILE_NAME "rules.lua"

namespace fs = std::filesystem;

namespace munchkin {
struct FlowEvent {
    enum class EventType {
        // Pushed at the end of every tick
        tick,
        // Pushed when any card is discarded
        card_discarded,
        // Pushed when any card is clicked
        card_clicked
    };
    EventType type;
    CardPtr card_involved;
    size_t player_id_involved = 0;
};

class State;
class State {
public:
    State(size_t player_count, std::string gamerule_path = DEFAULT_GAMERULES_PATH);

    // api functions
    // More information in scripting_api.md

    int get_ticks() const;
    void add_coroutine(sol::function);

    void give_treasure(Player& player);
    void give_dungeon(Player& player);

    void start_battle();
    void end_current_battle();

    Player& get_player(size_t id);
    PlayerPtr get_current_player() { return PlayerPtr(*this, current_player_id); }
    size_t get_player_count() const;
    void set_current_player(size_t id);
    // next_player_turn() defined in api_wrapper.lua

    std::vector<CardPtr> get_visible_cards();
    std::vector<CardPtr> get_all_cards();

    void push_event(FlowEvent::EventType type, CardPtr card_involved, PlayerPtr player_involved);

    // data

    sol::state lua;
    sol::table game_api;
    bool should_borrow_facing_up;

    std::string get_last_game_stage();
    std::string get_game_stage();
    void set_game_stage(std::string);

    size_t turn_number = 1;
    size_t tick = 0;

    size_t generate_id() { return last_id++; }
    size_t last_id = 0;

    std::vector<Player> players;
    size_t current_player_id;

    // Loads a cardpack from a path. The path must be the directory of the cardpack.
    void add_cardpack(fs::path path);
    Card& add_card(CardDef& def);
    std::vector<Card> all_cards;

    std::vector<CardPtr> dungeon_deck;
    std::vector<CardPtr> dungeon_discard_deck;
    std::vector<CardPtr> treasure_deck;
    std::vector<CardPtr> treasure_discard_deck;

    // Dumb lua API wrappers for decks
    CardPtr get_dungeon_deck_front() { return dungeon_deck.back(); }
    int get_dungeon_deck_size() { return dungeon_deck.size(); }
    void dungeon_deck_pop() { return dungeon_deck.pop_back(); }

    CardPtr get_dungeon_discard_deck_front() { return dungeon_discard_deck.back(); }
    int get_dungeon_discard_deck_size() { return dungeon_discard_deck.size(); }
    void dungeon_discard_deck_pop() { return dungeon_discard_deck.pop_back(); }

    CardPtr get_treasure_deck_front() { return treasure_deck.back(); }
    int get_treasure_deck_size() { return treasure_deck.size(); }
    void treasure_deck_pop() { return treasure_deck.pop_back(); }

    CardPtr get_treasure_discard_deck_front() { return treasure_discard_deck.back(); }
    int get_treasure_discard_deck_size() { return treasure_discard_deck.size(); }
    void treasure_discard_deck_pop() { return treasure_discard_deck.pop_back(); }

    std::queue<FlowEvent> event_queue;
    FlowEvent last_event;
    std::vector<sol::coroutine> active_coroutines;

    size_t default_hand_max_cards;

    std::shared_ptr<Battle> current_battle;

    std::shared_ptr<Battle> get_current_battle() { return current_battle; }

private:
    std::string last_game_stage;
    std::string game_stage;
};

} // namespace munchkin

#endif