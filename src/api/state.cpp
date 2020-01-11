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

    lua.new_enum<FlowEvent::EventType>("event_type",
        { {"tick", FlowEvent::EventType::tick},
        {"clicked_dungeon_deck", FlowEvent::EventType::clicked_dungeon_deck},
        {"card_discarded", FlowEvent::EventType::card_discarded},
        {"card_played", FlowEvent::EventType::card_played} });

    // register types in lua api
    lua.new_usertype<FlowEvent>("flow_event",
        "type", &FlowEvent::type);

    // TODO: Rename to munchkin_game OR rename game to state
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
        "get_player_count", &State::get_player_count,
        "get_current_player", &State::get_current_player,
        "set_current_player", &State::set_current_player,
        // next_player_turn defined in api_wrapper
        "get_visible_cards", &State::get_visible_cards,

        "default_hand_max_cards", &State::default_hand_max_cards
    );

    lua.new_usertype<Player>("munchkin_player",
        "level", &Player::level,
        "id", &Player::id,
        "hand", &Player::hand,
        "hand_max_cards", &Player::hand_max_cards,
        "equipped", &Player::equipped
    );

    lua.new_usertype<Battle>("munchkin_battle",
        "player_power_offset", &Battle::player_power_offset,
        "monster_power_offset", &Battle::monster_power_offset,
        "get_total_player_power", &Battle::get_total_player_power,
        "get_total_monster_power", &Battle::get_total_monster_power
    );

    lua.open_libraries(sol::lib::coroutine);
    
    // Load the generic API wrapper
    lua["game"] = this;
    lua.script_file(STATE_API_WRAPPER_FILE_PATH);

    game_api = lua["game"];

    // Load the gamerule's API
    std::filesystem::path fspath(gamerule_path);
    fspath /= STATE_API_RULES_FILE_NAME;
    lua.script_file(fspath.string());

    game_api["init_rules"](this);
    for (auto& player : players) {
        player.hand_max_cards = default_hand_max_cards;
    }

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
    player.hand.emplace_back(treasure_deck.front());
    treasure_deck.pop();
}

void State::give_dungeon(Player& player)
{
    player.hand.emplace_back(dungeon_deck.front());
    dungeon_deck.pop();
}

void State::open_dungeon()
{
    if (dungeon_deck.size() == 0) return;

    sol::function on_reveal = dungeon_deck.front()->get_data_variable("on_reveal");

    if (on_reveal == sol::lua_nil) {
        give_dungeon(get_current_player());
    }
    else {
        active_coroutines.emplace_back(on_reveal);
        dungeon_deck.pop();
        on_reveal();
    }
}

void State::start_battle()
{
    if (current_battle)
        return;

    current_battle = Battle(get_current_player());
}

void State::end_current_battle()
{
    if (!current_battle)
        return;

    current_battle.reset();
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

std::vector<CardPtr> State::get_visible_cards()
{
    std::vector<CardPtr> result;

    if (dungeon_deck.size() > 0)
        result.emplace_back(dungeon_deck.front());
    if (treasure_deck.size() > 0)
        result.emplace_back(treasure_deck.front());
    if (dungeon_discard_deck.size() > 0)
        result.emplace_back(dungeon_discard_deck.front());
    if (treasure_discard_deck.size() > 0)
        result.emplace_back(treasure_discard_deck.front());

    for (auto& player : players)
    {
        for (auto& card : player.hand)
            result.emplace_back(card);
        for (auto& card : player.equipped)
            result.emplace_back(card);
    }

    return result;
}

void State::add_cardpack(std::string path)
{
    std::vector<CardDef> new_carddefs = load_cards(path, lua);
    for (auto& def : new_carddefs)
    {
        carddefs.emplace_back(def);
        add_card(def);
    }
}

Card& State::add_card(CardDef& def)
{
    Card card(*this, def, {});
    Card& result = all_cards.emplace_back(std::move(card));
    switch (def.category) {
        case DeckType::dungeon:
            dungeon_deck.push(result);
            break;

        case DeckType::treasure:
            treasure_deck.push(result);
            break;
    }

    return result;
}

size_t State::get_player_count() const {
    return player_count;
}

}