#include "api/state.hpp"

#include "api/card_loader.hpp"

#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace munchkin {

State::State(size_t player_count, std::string gamerule_path) : player_count(player_count) {
    // create players
    players.resize(player_count);
    for (int i = 0; i < player_count; ++i) { players[i].id = i; }
    current_player_id = 0;

    /* clang-format off */
    lua.new_enum<FlowEvent::EventType>("event_type",
                                       {{"tick", FlowEvent::EventType::tick},
                                        {"card_discarded", FlowEvent::EventType::card_discarded},
                                        {"card_clicked", FlowEvent::EventType::card_clicked}});

    // register types in lua api
    lua.new_usertype<FlowEvent>("flow_event",
        "type", &FlowEvent::type,
        "card_involved", &FlowEvent::card_involved,
        "player_id_involved", &FlowEvent::player_id_involved);

    // @todo: Rename to munchkin_game OR rename game to state
    sol::usertype<State> state_type = lua.new_usertype<State>("munchkin_state",
        "last_event", &State::last_event,
        "get_ticks", &State::get_ticks,
        "stage", sol::property(&State::get_game_stage, &State::set_game_stage),
        "turn_number", &State::turn_number,
        "add_coroutine", &State::add_coroutine,

        "give_treasure", &State::give_treasure,
        "give_dungeon", &State::give_dungeon,
        "should_borrow_facing_up", &State::should_borrow_facing_up,

        "start_battle", &State::start_battle,
        "current_battle", sol::readonly_property(&State::get_current_battle),
        "end_current_battle", &State::end_current_battle,

        "get_player", &State::get_player,
        "get_player_count", &State::get_player_count,
        "get_current_player", &State::get_current_player,
        "set_current_player", &State::set_current_player,
        // next_player_turn defined in api_wrapper
        "get_visible_cards", &State::get_visible_cards,
        "all_cards", sol::readonly_property(&State::get_all_cards),

        "get_dungeon_deck_front", &State::get_dungeon_deck_front,
        "get_dungeon_deck_size", &State::get_dungeon_deck_size,
        "dungeon_deck_pop", &State::dungeon_deck_pop,

        "get_dungeon_discard_deck_front", &State::get_dungeon_discard_deck_front,
        "get_dungeon_discard_deck_size", &State::get_dungeon_discard_deck_size,
        "dungeon_discard_deck_pop", &State::dungeon_discard_deck_pop,

        "get_treasure_deck_front", &State::get_treasure_deck_front,
        "get_treasure_deck_size", &State::get_treasure_deck_size,
        "treasure_deck_pop", &State::treasure_deck_pop,

        "get_treasure_discard_deck_front", &State::get_treasure_discard_deck_front,
        "get_treasure_discard_deck_size", &State::get_treasure_discard_deck_size,
        "treasure_discard_deck_pop", &State::treasure_discard_deck_pop,

        "default_hand_max_cards", &State::default_hand_max_cards);

    lua.new_usertype<Player>("munchkin_player",
        "level", &Player::level,
        "id", &Player::id,
        "hand", &Player::hand,
        "hand_max_cards", &Player::hand_max_cards,
        "equipped", &Player::equipped);

    lua.new_usertype<Battle>("munchkin_battle",
        "player_power_offset", &Battle::player_power_offset,
        "monster_power_offset", &Battle::monster_power_offset,
        "get_total_player_power", &Battle::get_total_player_power,
        "get_total_monster_power", &Battle::get_total_monster_power,
        "add_card", &Battle::add_card,
        "remove_card", &Battle::remove_card,
        "modify_card", &Battle::modify_card,
        "get_cards_played", &Battle::get_cards_played,
        "get_card_power", &Battle::get_card_power,
        
        "treasures_to_draw", &Battle::treasures_to_draw,
        "levels_to_obtain", &Battle::levels_to_obtain);

    lua.new_enum<Card::CardVisibility>(
        "card_visibility",
        {{"back_visible", Card::CardVisibility::back_visible},
         {"front_visible", Card::CardVisibility::front_visible},
         {"front_visible_to_owner", Card::CardVisibility::front_visible_to_owner}});

    lua.new_enum<Card::CardLocation>(
        "card_location", {{"invalid", Card::CardLocation::invalid},
                          {"dungeon_deck", Card::CardLocation::dungeon_deck},
                          {"dungeon_discard_deck", Card::CardLocation::dungeon_discard_deck},
                          {"treasure_deck", Card::CardLocation::treasure_deck},
                          {"treasure_discard_deck", Card::CardLocation::treasure_discard_deck},
                          {"player_equipped", Card::CardLocation::player_equipped},
                          {"player_hand", Card::CardLocation::player_hand},
                          {"table_center", Card::CardLocation::table_center}});

    lua.new_enum<DeckType>("munchkin_deck_type", {{"dungeon", DeckType::dungeon},
                                                  {"treasure", DeckType::treasure},
                                                  {"null", DeckType::null}});

    lua.new_usertype<CardDef>("munchkin_card_def",
        "name", &CardDef::name,
        "description", &CardDef::description,
        "category", &CardDef::category,
        "play_stages", &CardDef::play_stages);

    lua.new_usertype<Card>("munchkin_card",
        "get_id", &Card::get_id,
        "get_def", &Card::get_def,
        "visibility", &Card::visibility,
        "get_location", &Card::get_location,
        "move_to", &Card::move_to,
        "owner_id", &Card::owner_id,
        sol::meta_function::index, &Card::get_data_variable,
        sol::meta_function::new_index, &Card::set_data_variable);

    /* clang-format on */

    lua.open_libraries(sol::lib::coroutine);
    lua.open_libraries(sol::lib::base);

    // Load the generic API wrapper
    lua["game"] = this;
    lua.script_file(STATE_API_WRAPPER_FILE_PATH);

    game_api = lua["game"];

    // Load the gamerule's API
    std::filesystem::path fspath(gamerule_path);
    fspath /= STATE_API_RULES_FILE_NAME;
    lua.script_file(fspath.string());

    game_api["init_rules"](this);
    for (auto& player : players) { player.hand_max_cards = default_hand_max_cards; }
}

int State::get_ticks() const { return tick; }

void State::give_treasure(Player& player) {
    if (treasure_deck.size() == 0)
        return;
    CardPtr ptr = treasure_deck.back();
    ptr->move_to(Card::CardLocation::player_hand, player.id);
    ptr->visibility = Card::CardVisibility::front_visible_to_owner;
}

void State::give_dungeon(Player& player) {
    if (dungeon_deck.size() == 0)
        return;
    CardPtr ptr = dungeon_deck.back();
    ptr->move_to(Card::CardLocation::player_hand, player.id);
    ptr->visibility = Card::CardVisibility::front_visible_to_owner;
}

void State::start_battle() {
    if (current_battle)
        return;

    current_battle = std::make_shared<Battle>(*this, get_current_player());
}

void State::end_current_battle() {
    if (!current_battle)
        return;

    current_battle.reset();
}

Player& State::get_player(size_t id) { return players.at(id); }

Player& State::get_current_player() { return players[current_player_id]; }

void State::set_current_player(size_t id) { current_player_id = id; }

std::vector<CardPtr> State::get_visible_cards() {
    std::vector<CardPtr> result;

    if (dungeon_deck.size() > 0)
        result.emplace_back(dungeon_deck.front());
    if (treasure_deck.size() > 0)
        result.emplace_back(treasure_deck.front());
    if (dungeon_discard_deck.size() > 0)
        result.emplace_back(dungeon_discard_deck.front());
    if (treasure_discard_deck.size() > 0)
        result.emplace_back(treasure_discard_deck.front());

    for (auto& player : players) {
        for (auto& card : player.hand) result.emplace_back(card);
        for (auto& card : player.equipped) result.emplace_back(card);
    }

    return result;
}

std::vector<CardPtr> State::get_all_cards() {
    std::vector<CardPtr> cards;
    for (auto& card : all_cards) cards.emplace_back(&card);
    return cards;
}

std::string State::get_last_game_stage() { return last_game_stage; }

std::string State::get_game_stage() { return game_stage; }

void State::set_game_stage(std::string s) {
    last_game_stage = game_stage;
    game_stage = s;
}

void State::add_cardpack(std::string path) {
    std::vector<CardDef> new_carddefs = load_cards(path, lua);
    for (auto& def : new_carddefs) add_card(def);
}

Card& State::add_card(CardDef& def) {
    Card card(*this, def, {});
    Card& result = all_cards.emplace_back(std::move(card));
    switch (def.category) {
        case DeckType::dungeon: result.move_to(Card::CardLocation::dungeon_deck); break;

        case DeckType::treasure: result.move_to(Card::CardLocation::treasure_deck); break;

        default: break;
    }

    return result;
}

size_t State::get_player_count() const { return player_count; }

void State::add_coroutine(sol::function coro) { active_coroutines.emplace_back(coro); }

} // namespace munchkin