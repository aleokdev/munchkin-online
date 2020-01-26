#include "api/card.hpp"

#include "api/player.hpp"
#include "api/state.hpp"
#include <algorithm>
#include <iostream>

namespace munchkin {

Card::Card(State& st, CardDef& def, ConstructorKey) :
    state(&st), def(def), data(sol::state_view(def.metatable.lua_state()).create_table()) {
    id = st.generate_id();

    // @todo: Replace with metatables (HOW?)
    for (auto& [key, val] : def.metatable) { data[key] = val; }

    data["id"] = id;
}

CardPtr::CardPtr(Card& card) : state(&card.get_state()), card_id(card.get_id()) {}

CardPtr::CardPtr(State& _state, size_t cardID) : state(&_state), card_id(cardID) {}

CardPtr::operator Card*() const {
    Card* retval = nullptr;
    for (auto& card : state->all_cards) {
        if (card.get_id() == card_id) {
            retval = std::addressof(card);
            break;
        }
    }
    return retval;
}

Card* CardPtr::operator->() const { return *this; }

CardPtr Card::operator&() { return CardPtr(*this); }

void Card::move_to(Card::CardLocation loc, int _owner_id) {
    owner_id = _owner_id;

    // Clean up before moving
#define REMOVE_FROM_CONTAINER(c) c.erase(std::remove(c.begin(), c.end(), &*this), c.end())
    switch (location) {
        case munchkin::Card::CardLocation::invalid: break;

        case munchkin::Card::CardLocation::dungeon_deck:
            REMOVE_FROM_CONTAINER(state->dungeon_deck);
            break;

        case munchkin::Card::CardLocation::dungeon_discard_deck:
            REMOVE_FROM_CONTAINER(state->dungeon_discard_deck);
            break;

        case munchkin::Card::CardLocation::treasure_deck:
            REMOVE_FROM_CONTAINER(state->treasure_deck);
            break;

        case munchkin::Card::CardLocation::treasure_discard_deck:
            REMOVE_FROM_CONTAINER(state->treasure_discard_deck);
            break;

        case munchkin::Card::CardLocation::player_equipped:
            REMOVE_FROM_CONTAINER(state->players[owner_id].equipped);
            break;

        case munchkin::Card::CardLocation::player_hand:
            REMOVE_FROM_CONTAINER(state->players[owner_id].hand);
            break;

        case munchkin::Card::CardLocation::table_center: {
            // Could be probably on a battle
            if (!state->current_battle)
                break;

            auto iter = state->current_battle->played_cards.find(&*this);
            if (iter != state->current_battle->played_cards.end()) {
                state->current_battle->played_cards.erase(iter);
            }
            break;
        }
    }
#undef REMOVE_FROM_CONTAINER

    // Move to given location
    location = loc;

    switch (location) {
        case munchkin::Card::CardLocation::invalid: break;

        case munchkin::Card::CardLocation::dungeon_deck:
            state->dungeon_deck.emplace_back(&*this);
            break;

        case munchkin::Card::CardLocation::dungeon_discard_deck:
            state->dungeon_discard_deck.emplace_back(&*this);
            break;

        case munchkin::Card::CardLocation::treasure_deck:
            state->treasure_deck.emplace_back(&*this);
            break;

        case munchkin::Card::CardLocation::treasure_discard_deck:
            state->treasure_discard_deck.emplace_back(&*this);
            break;

        case munchkin::Card::CardLocation::player_equipped:
            state->players[owner_id].equipped.emplace_back(&*this);
            break;

        case munchkin::Card::CardLocation::player_hand:
            state->players[owner_id].hand.emplace_back(&*this);
            break;

        case munchkin::Card::CardLocation::table_center:
            // Do nothing: If the user wants this card to be moved into a battle, it must be done
            // explicitly.
            break;
    }
}

Card::CardLocation Card::get_location() { return location; }

} // namespace munchkin