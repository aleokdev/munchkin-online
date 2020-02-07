#include "api/player.hpp"
#include "api/state.hpp"

#include <exception>

namespace munchkin {
Player::Player(State& s, size_t _id) : state(&s), id(_id) {}

Player* PlayerPtr::operator->() const {
    if (state == nullptr)
        throw std::runtime_error("Tried to index null PlayerPtr!");
    Player* retval = nullptr;
    for (auto& player : state->players) {
        if (player.get_id() == player_id) {
            retval = std::addressof(player);
            break;
        }
    }
    return retval;
}

PlayerPtr Player::operator&() { return PlayerPtr(*state, id); }

} // namespace munchkin