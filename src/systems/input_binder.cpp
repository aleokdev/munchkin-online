#include "systems/input_binder.hpp"
#include "game.hpp"

namespace munchkin {
namespace systems {

InputBinder::InputBinder(Game& g) : game(&g) {}

void InputBinder::tick() {
	for (auto& sprite : game->card_sprites) {

	}
}

}
}