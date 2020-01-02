#include "api/game.hpp"

namespace munchkin {

Game::Game(size_t player_count) : state(player_count) {

}

void Game::turn() {
    // call on_play with state and player who played the card
    state.lua["card"]["on_play"](state, state.players[0]);
}

bool Game::ended() const {
    return state.game_ended;
}

}