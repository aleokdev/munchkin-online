#include "api/game.hpp"

#include "api/card.hpp"

#include <iostream>

namespace munchkin {

Game::Game(size_t player_count, std::string gamerules_path) : state(player_count), gamerules(state.lua, gamerules_path) {
    // Get the first game stage
    tick();
}

void Game::turn() {
    std::cout << "Turn " << state.turn_number << std::endl
              << "Player " << state.get_current_player().id << "'s turn" << std::endl
              << "Stage: " << state.game_stage << std::endl;

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
    while (state.event_queue.size() > 0)
    {
        state.last_event = state.event_queue.front();
        gamerules.continue_flow();
        state.event_queue.pop();
    }
}

void Game::push_event(FlowEvent e)
{
    state.event_queue.push(e);
}

bool Game::ended() {
    return state.game_api["has_ended"](state.game_api);
}

}