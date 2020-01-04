#ifndef MUNCHKIN_CARD_HPP_
#define MUNCHKIN_CARD_HPP_

#include <sol/sol.hpp>
#include <string>
#include "api/carddef.hpp"

namespace munchkin {

class State;
class Player;

// Cards refer to CardDefs
class Card {
public:
    // Object is unique so no copy operators
    Card(Card const&) = delete;
    Card(Card&) = delete;
    Card(Card&&) = default;

    Card& operator=(Card const&) = delete;
    Card& operator=(Card&&) = delete;

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

    // We use the passkey idiom to not allow State to access *everything* inside card
    class ConstructorKey {
        friend class State;
    private:
        ConstructorKey();
        ConstructorKey(ConstructorKey const&) = default;
    };

protected:
    // Constructor (Only reserved to the Card Factory: State)
    Card(State&, CardDef&, ConstructorKey);

private:
    State& state;
    CardDef* def;
    sol::table data;
    static inline size_t last_id = 0;
    size_t id;
};

// CardPtrs refer to the ID of a card. This is basically a safer version of Card*, because addresses aren't involved.
class CardPtr {
    CardPtr(Card& card);
    CardPtr(State& _state, size_t cardID);

    operator Card*() const;
    Card* operator->() const;

    size_t card_id;
    State& state;
};

}

#endif