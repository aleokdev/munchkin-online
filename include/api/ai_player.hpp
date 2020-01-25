#ifndef MUNCHKIN_AI_PLAYER_HPP__
#define MUNCHKIN_AI_PLAYER_HPP__

namespace munchkin {
class State;
}

namespace munchkin {
namespace games {

class AIPlayer {
public:
    AIPlayer(State&, size_t attached_id);

    void tick();

private:
    State* state;
    size_t player_id;

    const int ticks_to_think = 40;
    int last_action_tick = -1;
};

} // namespace games
} // namespace munchkin
#endif