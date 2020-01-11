#include "api/game.hpp"

#include "api/card.hpp"

#include <iostream>
#include <algorithm>

namespace munchkin {

Game::Game(size_t player_count, std::string gamerules_path) : state(player_count), gamerules(state, gamerules_path) {
    // Get the first game stage by executing the active coroutines (Which, right now, only includes the gamerules' game_flow)
    tick();
}

void Game::turn() {
    std::cout << "Turn " << state.turn_number << "\n"
              << "Player " << state.get_current_player().id << "'s turn\n"
              << "Stage: " << state.game_stage << "\n"
              << "Active coroutines: " << state.active_coroutines.size() << std::endl;

    // check if the game is over
    sol::object result = state.game_api["get_winner"](state.game_api);
    if (result != sol::lua_nil) {
        // We have a winner!
        Player& player = result.as<Player>();
        std::cout << "The winner is: " << player.id << "\n";
    }

    // Push an event, and in return, calculate the game stage
    std::cout << "Input event: ";
    std::string event_to_push;
    std::cin >> event_to_push;
    state.event_queue.push({ event_to_push });
    tick();
}

void Game::tick()
{
    state.event_queue.push({ "tick" });
    state.tick++;
    while (state.event_queue.size() > 0) {
        state.last_event = state.event_queue.front();
        if (state.last_event.name != "tick")
            std::cout << "Processed event of name " << state.last_event.name << std::endl;
        for (auto& coroutine : state.active_coroutines) {
            if (!coroutine.runnable()) {
                std::cout << "Encountered dead coroutine, need to discard it from active_coroutines." << std::endl;
            }
            else {
                auto result = coroutine();
                if (!result.valid())
                {
                    std::cout << "Runtime error in coroutine!!" << std::endl;
                    sol::script_throw_on_error(state.lua, result);
                }
            }
        }
        state.event_queue.pop();
    }

    // Discard dead coroutines
    state.active_coroutines.erase(std::remove_if(state.active_coroutines.begin(), state.active_coroutines.end(),
        [](sol::coroutine const& coro) { return !coro.runnable(); }
    ), state.active_coroutines.end());
}

void Game::push_event(FlowEvent e)
{
    state.event_queue.push(e);
}

bool Game::ended() {
    return state.game_api["has_ended"](state.game_api);
}

}