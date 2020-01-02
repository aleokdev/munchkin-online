#include "api/game.hpp"

#include "api/card.hpp"

namespace munchkin {

Game::Game(size_t player_count) : state(player_count) {
    
}

void Game::turn() {
    // advance to next player's turn
    state.next_player_turn();

    // check if the game is over
    sol::object result = state.game_api["winner"]();
    if (result != sol::lua_nil) {
        // We have a winner!
        Player& player = result.as<Player>();
        std::cout << "The winner is: " << player.id << "\n";
    }
}

bool Game::ended() {
    return state.game_api["has_ended"]();
}

}