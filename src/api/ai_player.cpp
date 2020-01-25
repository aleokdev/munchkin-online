#include "api/ai_player.hpp"
#include "api/state.hpp"

namespace munchkin {
namespace games {

AIPlayer::AIPlayer(State& s, size_t attached_id) : state(&s), player_id(attached_id) {}

void AIPlayer::tick() {
    if (state->current_player_id != player_id)
        return;

    if (last_action_tick == -1)
        last_action_tick = state->tick;

    if (state->tick > (last_action_tick + ticks_to_think)) {
        state->event_queue.push(
            FlowEvent{FlowEvent::EventType::card_clicked, state->dungeon_deck[0]});
        last_action_tick = -1;
    }
}

} // namespace games
} // namespace munchkin