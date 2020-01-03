#include "api/game.hpp"

#include "api/card.hpp"

#include <iostream>

namespace munchkin {

Game::Game(size_t player_count, std::string gamerules_path) : state(player_count), gamerules(state.lua, gamerules_path) {
    // Get the first game stage
    tick("game_start");
}

void Game::turn() {
    std::cout << "Turn " << state.turn_number << std::endl
              << "Player " << state.get_current_player().id << "'s turn" << std::endl
              << "Stage: " << game_stage << std::endl;

    // check if the game is over
    sol::object result = state.game_api["winner"]();
    if (result != sol::lua_nil) {
        // We have a winner!
        Player& player = result.as<Player>();
        std::cout << "The winner is: " << player.id << "\n";
    }

    // Push an event, and in return, calculate the game stage
    std::cout << "Input event: ";
    std::string event_to_push;
    std::cin >> event_to_push;
    tick(event_to_push);
}

void Game::tick(std::string event_name)
{
    state.last_event.name = event_name;
    gamerules.continue_flow();
    game_stage = state.lua["game"]["stage"];
}

bool Game::ended() {
    return state.game_api["has_ended"]();
}

}