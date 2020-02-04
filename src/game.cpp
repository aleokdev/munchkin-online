#include "game.hpp"

#include "api/card.hpp"

#include <algorithm>
#include <iostream>

namespace munchkin {

Game::Game(size_t player_count, size_t w, size_t h, std::string gamerules_path) :
    window_w(w), window_h(h), state(player_count), gamerules(state, gamerules_path),
    camera(-1, -1) {
    // Get the first game stage by executing the active coroutines (Which, right now, only includes
    // the gamerules' game_flow)
    tick();
}

void Game::turn() {
    std::cout << "Turn " << state.turn_number << "\n"
              << "Player " << state.get_current_player()->get_id() << "'s turn\n"
              << "Stage: " << state.get_game_stage() << "\n"
              << "Active coroutines: " << state.active_coroutines.size() << std::endl;

    // check if the game is over
    sol::object result = state.game_api["get_winner"](state.game_api);
    if (result != sol::lua_nil) {
        // We have a winner!
        Player& player = result.as<Player>();
        std::cout << "The winner is: " << player.get_id() << "\n";
    }
    tick();
}

void Game::tick() {
    state.event_queue.push({FlowEvent::EventType::tick});
    state.tick++;
    std::vector<sol::coroutine> last_coroutines = state.active_coroutines;
    while (state.event_queue.size() > 0) {
        state.last_event = state.event_queue.front();
        for (auto& coroutine : last_coroutines) {
            if (!coroutine.runnable()) {
                std::cout
                    << "Encountered dead coroutine, need to discard it from active_coroutines."
                    << std::endl;
            } else {
                auto result = coroutine();
                if (!result.valid()) {
                    std::cout << "Runtime error in coroutine!!" << std::endl;
                    sol::script_throw_on_error(state.lua, result);
                } else if (result.status() == sol::call_status::ok) {
                    std::cout
                        << "Encountered finished coroutine, removing it from active_coroutines..."
                        << std::endl;
                    state.active_coroutines.erase(std::remove(state.active_coroutines.begin(),
                                                              state.active_coroutines.end(),
                                                              coroutine),
                                                  state.active_coroutines.end());
                }
            }
        }
        state.event_queue.pop();
    }

    // Discard dead coroutines
    state.active_coroutines.erase(
        std::remove_if(state.active_coroutines.begin(), state.active_coroutines.end(),
                       [](sol::coroutine const& coro) { return !coro.runnable(); }),
        state.active_coroutines.end());
}

void Game::push_event(FlowEvent e) { state.event_queue.push(e); }

bool Game::ended() { return state.game_api["has_ended"](state.game_api); }

} // namespace munchkin