#ifndef MUNCHKIN_PLAYER_HPP_
#define MUNCHKIN_PLAYER_HPP_

#include <sol/sol.hpp>
#include <vector>

namespace munchkin {

class State;
struct CardPtr;
struct PlayerPtr;

class Player {
public:
    Player(State&, size_t id);

    int level = 1;
    size_t get_id() { return id; }

    int get_power() { return level; }
    State& get_state() { return *state; }

    PlayerPtr operator&();

    std::vector<CardPtr> hand;
    std::vector<CardPtr> equipped;
    size_t hand_max_cards;
    std::string name;

private:
    size_t id;
    State* state;
};

struct PlayerPtr {
    State* state;
    size_t player_id;

    PlayerPtr() : state(nullptr), player_id(0){};
    PlayerPtr(nullptr_t) : state(nullptr), player_id(0){};
    PlayerPtr(Player& player) : state(&player.get_state()), player_id(player.get_id()){};
    PlayerPtr(State& _state, size_t playerID) : state(&_state), player_id(playerID){};
    PlayerPtr(PlayerPtr&) = default;
    PlayerPtr(PlayerPtr const&) = default;
    PlayerPtr& operator=(PlayerPtr const&) = default;
    PlayerPtr& operator=(PlayerPtr&&) = default;
    PlayerPtr& operator=(nullptr_t) {
        state = nullptr;
        return *this;
    }

    void reset() { state = nullptr; }

    Player* operator->() const;
    Player& operator*() const { return *this->operator->(); }
    Player* get() const { return this->operator->(); }

    operator bool() const { return *this == nullptr; }
    bool operator==(PlayerPtr const& b) const { return player_id == b.player_id; }
    bool operator==(nullptr_t) const { return state == nullptr; }
};

} // namespace munchkin

namespace std {
template<> struct hash<munchkin::PlayerPtr> {
    size_t operator()(const munchkin::PlayerPtr& playerptr) const {
        return reinterpret_cast<std::uintptr_t>(playerptr.get());
    }
};
} // namespace std

namespace sol {
template<> struct unique_usertype_traits<munchkin::PlayerPtr> {
    typedef munchkin::Player type;
    typedef munchkin::PlayerPtr actual_type;
    static const bool value = true;

    static bool is_null(const actual_type& ptr) { return ptr == nullptr; }
    static type* get(const actual_type& ptr) { return ptr.get(); }
};
} // namespace sol

#endif