#ifndef MUNCHKIN_AI_MANAGER_HPP__
#define MUNCHKIN_AI_MANAGER_HPP__

#include "ai_player.hpp"
#include <vector>

namespace munchkin {
namespace games {

class AIManager {
public:
    AIManager(State&, std::vector<size_t> players_to_control);

    void tick();

private:
    State* state;
    std::vector<AIPlayer> ais;
};

} // namespace games
} // namespace munchkin

#endif