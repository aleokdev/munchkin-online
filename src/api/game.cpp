#include "api/game.hpp"

#include "api/card.hpp"

#include <iostream>

namespace munchkin {

Game::Game(size_t player_count) : state(player_count) {
    
}

void Game::turn() {
    std::cout << "Next turn...!" << std::endl;

    // check if the game is over
    sol::object result = state.game_api["winner"]();
    if (result != sol::lua_nil) {
        // We have a winner!
        Player& player = result.as<Player>();
        std::cout << "The winner is: " << player.id << "\n";
    }

    // advance to next player's turn
    state.next_player_turn();
}

bool Game::ended() {
    return state.game_api["has_ended"]();
}

}