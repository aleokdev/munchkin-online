#include "api/player.hpp"
#include "api/state.hpp"

#include <exception>

namespace munchkin {
Player::Player(State& s, size_t _id) : state(&s), id(_id) {}

Player* PlayerPtr::operator->() const {
    if (state == nullptr)
        throw std::runtime_error("Tried to index null CardPtr!");
    Player* retval = nullptr;
    for (auto& player : state->players) {
        if (player.id == player_id) {
            retval = std::addressof(player);
            break;
        }
    }
    return retval;
}

} // namespace munchkin