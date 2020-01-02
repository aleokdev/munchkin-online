#include "api/game.hpp"

#include "api/card.hpp"

namespace munchkin {

Game::Game(size_t player_count) : state(player_count) {
    
}

void Game::turn() {
    size_t player_id = state.current_player_id;

    Player& player = state.get_player(player_id);

    // advance to next player's turn
    state.current_player_id++;
    state.current_player_id %= state.player_count;

    // check if the game is over
    sol::object result = state.game_api["has_game_ended"](state);
    if (result != sol::lua_nil) {
        // We have a winner!
        Player& player = result.as<Player>();
        std::cout << "The winner is: " << player.id << "\n";
        state.game_ended = true;
    }
}

bool Game::ended() const {
    return state.game_ended;
}

}