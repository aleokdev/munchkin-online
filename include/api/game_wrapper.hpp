#ifndef MUNCHKIN_GAME_WRAPPER_HPP__
#define MUNCHKIN_GAME_WRAPPER_HPP__

#include "renderer/game_renderer.hpp"
#include "gamerules.hpp"
#include "state.hpp"

namespace munchkin {

template<typename T>
class GameWrapper {
public:
	GameWrapper(size_t window_w, size_t window_h, int players_count, std::string gamerules_path = DEFAULT_GAMERULES_PATH) : game(players_count, gamerules_path), renderer(game.get_state(), window_w, window_h) {}

	void render() {
		renderer.render_frame();

		renderer.blit(0);
		glBindFramebuffer(GL_FRAMEBUFFER, NULL);
	}

	void tick() {
		game.tick();
	}

	void push_event(FlowEvent event) {
		game.get_state().event_queue.push(event);
	}

	State& get_state() {
		return game.get_state();
	}

	T game;
	GameRenderer renderer;
};

}





#endif