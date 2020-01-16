#ifndef MUNCHKIN_GAME_HPP_
#define MUNCHKIN_GAME_HPP_

#include "api/state.hpp"
#include "api/gamerules.hpp"
#include "renderer/card_sprite.hpp"

namespace munchkin {

class Game {
public:
    Game(size_t player_count, std::string gamerules_path = DEFAULT_GAMERULES_PATH);

    void turn();

    void tick();

    void push_event(FlowEvent e);

    bool ended();

    State& get_state() { return state; }

    State state;
    GameRules gamerules;

	// Sprites
	std::vector<renderer::CardSprite> card_sprites;
};

}

#endif