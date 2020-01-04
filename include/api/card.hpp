#ifndef MUNCHKIN_CARD_HPP_
#define MUNCHKIN_CARD_HPP_

#include <sol/sol.hpp>
#include <string>
#include "api/carddef.hpp"

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
        ConstructorKey() {};
        ConstructorKey(ConstructorKey const&) = default;
    };

    // Constructor (Only reserved to the Card Factory: State)
    Card(State&, CardDef&, ConstructorKey);

    Card(Card&&) = default;

    Card& operator=(Card const&) = default;
    Card& operator=(Card&&) = default;

    CardPtr operator&();

    ~Card() = default;

    CardDef& get_def() { return *def; }
    CardDef const& get_def() const { return *def; }

    template<typename... Args>
    sol::object execute_function(std::string_view name, Args&&... args) {
        return data[name](data, std::forward<Args>(args)...);
    }

    sol::object get_data_variable(std::string_view name) {
        return data[name];
    }

    size_t get_id() const {
        return id;
    }

    State& get_state() {
        return *state;
    }

private:
    State* state;
    CardDef* def;
    sol::table data;
    size_t id;
};

// CardPtrs refer to the ID of a card. This is basically a safer version of Card*, because addresses aren't involved.
struct CardPtr {
    CardPtr(Card& card);
    CardPtr(State& _state, size_t cardID);
    CardPtr(CardPtr&) = default;
    CardPtr(CardPtr const&) = default;
    CardPtr& operator=(CardPtr const&) = default;
    CardPtr& operator=(CardPtr&&) = default;

    operator Card*() const;
    Card* operator->() const;

    size_t card_id;
    State* state;
};

}

namespace std {
    template<>
    struct hash<munchkin::CardPtr> {
        size_t operator()(const munchkin::CardPtr& cardptr) const {
            return reinterpret_cast<std::uintptr_t>((munchkin::Card*)cardptr);
        }
    };
}

#endif