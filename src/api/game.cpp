#include "api/game.hpp"

#include "api/card.hpp"

#include <iostream>

namespace munchkin {

Game::Game(size_t player_count, std::string gamerules_path) : state(player_count), gamerules(state.lua, gamerules_path) {
    // Get the first game stage
    gamerules.continue_flow();
    game_stage = state.lua["game"]["stage"];
}

void Game::turn() {
    std::cout << "Turn " << state.turn_number << std::endl;
    std::cout << "Stage: " << game_stage << std::endl;
    // check if the game is over
    sol::object result = state.game_api["winner"]();
    if (result != sol::lua_nil) {
        // We have a winner!
        Player& player = result.as<Player>();
        std::cout << "The winner is: " << player.id << "\n";
    }

    // Calculate game stage
    gamerules.continue_flow();
    game_stage = state.lua["game"]["stage"];

    // (debug) wait for user input so we don't get infinite "next turn"s
    std::cin.get();

    std::cout << "Next turn...!" << std::endl;
    // advance to next player's turn
    state.turn_number++;
    if (state.current_player == state.players.end())
        state.current_player = state.players.begin();
    else
        state.current_player++;
}

bool Game::ended() {
    return state.game_api["has_ended"]();
}

}