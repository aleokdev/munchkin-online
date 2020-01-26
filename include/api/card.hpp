#ifndef MUNCHKIN_CARD_HPP_
#define MUNCHKIN_CARD_HPP_

#include "api/carddef.hpp"
#include <sol/sol.hpp>
#include <string>

namespace munchkin {

class State;
class Player;
struct CardPtr;

// Cards refer to CardDefs
class Card {
public:
    // We use the passkey idiom to not allow State to access *everything* inside card
    class ConstructorKey {
        friend class State;

    private:
        ConstructorKey(){};
        ConstructorKey(ConstructorKey const&) = default;
    };

    // Constructor (Only reserved to the Card Factory: State)
    Card(State&, CardDef&, ConstructorKey);

    Card(Card&&) = default;
    Card(Card const&) = default;

    Card& operator=(Card const&) = default;
    Card& operator=(Card&&) = default;

    CardPtr operator&();

    ~Card() = default;

    CardDef const& get_def() const { return def; }

    template<typename... Args> sol::object execute_function(std::string name, Args&&... args) {
        return data[name](data, std::forward<Args>(args)...);
    }

    sol::object get_data_variable(std::string name) { return data[name]; }

    void set_data_variable(std::string key, sol::object obj) { data[key] = obj; }

    size_t get_id() const { return id; }

    State& get_state() { return *state; }

    enum class CardLocation {
        invalid = 0,
        dungeon_deck = 1,
        dungeon_discard_deck = 2 << 0,
        treasure_deck = 2 << 1,
        treasure_discard_deck = 2 << 2,
        player_equipped = 2 << 3,
        player_hand = 2 << 4,
        table_center = 2 << 5
    };

    void move_to(CardLocation, int owner_id = 0);
    CardLocation get_location();
    bool is_being_owned_by_player() {
        return (int)location &
               ((int)CardLocation::player_equipped | (int)CardLocation::player_hand);
    }

    // The ID of the player that owns this card (If location is set to player_equipped or
    // player_hand)
    int owner_id = 0;

    enum class CardVisibility {
        // The back of the card is visible to everybody (Back facing top)
        back_visible,
        // The front of the card is visible to everybody (Front facing top)
        front_visible,
        // The front of the card is visible only to the card's owner (Set via owner_id) (Front
        // facing top), rest of the players see it as back_visible
        front_visible_to_owner
    };

    // Specifies what is visible about the card (The back or the front side)
    CardVisibility visibility = CardVisibility::back_visible;

private:
    State* state;
    CardDef def;
    sol::table data;
    size_t id;
    CardLocation location = CardLocation::invalid;
};

// CardPtrs refer to the ID of a card. This is basically a safer version of Card*, because addresses
// aren't involved.
struct CardPtr {
    CardPtr(Card& card);
    CardPtr(State& _state, size_t cardID);
    CardPtr(CardPtr&) = default;
    CardPtr(CardPtr const&) = default;
    CardPtr& operator=(CardPtr const&) = default;
    CardPtr& operator=(CardPtr&&) = default;

    operator Card*() const;
    Card* operator->() const;
    Card& get() { return **this; }

    State* state;
    size_t card_id;

    bool operator==(CardPtr const& b) { return card_id == b.card_id; }
};

} // namespace munchkin

namespace std {
template<> struct hash<munchkin::CardPtr> {
    size_t operator()(const munchkin::CardPtr& cardptr) const {
        return reinterpret_cast<std::uintptr_t>((munchkin::Card*)cardptr);
    }
};
} // namespace std

#endif